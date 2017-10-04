#include "pageloader.h"

#include <QNetworkReply>
#include <QRegularExpression>
#include <QNetworkCookieJar>
#include <QNetworkCookie>

PageLoader::PageLoader(QObject *parent) :
	QObject(parent),
	_nam(new QNetworkAccessManager(this)),
	_id(-1),
	_next(0),
	_max(0)
{}

void PageLoader::startLoading(int id, int firstChapter, int lastChapter)
{
	_id = id;
	_next = firstChapter;
	_max = lastChapter;

	//TODO setup cookie-store
	auto jar = _nam->cookieJar();

	downloadNext();
}

void PageLoader::downloadNext()
{
	if(_next > _max) {
		//TODO complete
		return;
	}

	QNetworkRequest request(QStringLiteral("https://proxer.me/read/%1/%2/en/1")
							.arg(_id)
							.arg(_next));
	auto reply = _nam->get(request);
	connect(reply, &QNetworkReply::finished,
			this, &PageLoader::pageReplyDone);
	emit updateProgress(_next, 0, tr("Loading chapter meta-data…"));
}

void PageLoader::pageReplyDone()
{
	auto reply = qobject_cast<QNetworkReply*>(sender());
	if(!reply)
		return;

	try {
		if(reply->error() != QNetworkReply::NoError)
			throw tr("Failed to load meta-data with error: %1").arg(reply->errorString());

		//read the html and find the element
		QTextStream stream(reply);
		auto data = stream.readAll();
		qDebug().noquote() << data;

		static QRegularExpression scriptRegex(QStringLiteral(R"__(<div [^>]+class=\"shariff\"[^>]*><\/div><script [^>]*>(.*)<\/script>)__"),
											  QRegularExpression::CaseInsensitiveOption |
											  QRegularExpression::DotMatchesEverythingOption |
											  QRegularExpression::OptimizeOnFirstUsageOption);
		auto match = scriptRegex.match(data);
		if(!match.hasMatch())
			throw tr("Unable to find data-script in page");

		auto script = match.captured(1);
		qDebug() << script;

		emit updateProgress(_next, 0, tr("Successfully downloaded meta-data"));
	} catch (QString &s) {
		emit updateProgress(_next, 0, s, true);
	}
}
