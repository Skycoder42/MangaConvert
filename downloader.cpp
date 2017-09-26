#include "downloader.h"

#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QFile>

Downloader::Downloader(QObject *parent) :
	QObject(parent),
	_baseUrl(),
	_pages(),
	_nam(new QNetworkAccessManager(this)),
	_tmpDir(new QTemporaryDir()),
	_progCnt(0)
{
	_nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

bool Downloader::setBaseUrl(const QString &url)
{
	if(!QUrl(url.arg("test")).isValid()) {
		qCritical() << "Invalid base url:" << url;
		return false;
	}
	_baseUrl = url;
	return true;
}

bool Downloader::parsePages(const QStringList &pages)
{
	QRegularExpression regex(QStringLiteral(R"__(^(\d+)-(\d+)$)__"));
	regex.optimize();
	foreach(auto page, pages) {
		auto match = regex.match(page);
		if(match.hasMatch()) {
			auto first = match.captured(1);
			auto second = match.captured(2);
			auto width = qMax(first.size(), second.size());

			bool ok;
			auto i = first.toInt(&ok);
			if(!ok) {
				qCritical() << "Invalid argument:" << page;
				return false;
			}
			auto total = second.toInt(&ok);
			if(!ok) {
				qCritical() << "Invalid argument:" << page;
				return false;
			}
			for(; i <= total; i++)
				_pages.append(QStringLiteral("%1").arg(i, width, 10, QLatin1Char('0')));
		} else
			_pages.append(page);
	}

	return true;
}

void Downloader::start()
{
	if(_pages.isEmpty()) {
		qWarning() << "No pages specified. Doing nothing";
		qApp->quit();
		return;
	}

	for(auto i = 0; i < _pages.size(); i++) {
		QUrl url = _baseUrl.arg(_pages[i]);
		if(!url.isValid()) {
			qCritical() << "Invalid generated page url:" << _baseUrl.arg(_pages[i]);
			qApp->exit(EXIT_FAILURE);
			return;
		}

		QNetworkRequest request(url);
		auto reply = _nam->get(request);
		reply->setProperty("index", i);
		connect(reply, &QNetworkReply::finished,
				this, &Downloader::replyDone);
	}
}

void Downloader::replyDone()
{
	auto reply = qobject_cast<QNetworkReply*>(sender());
	if(!reply)
		return;

	auto index = reply->property("index").toInt();
	auto suffix = QFileInfo(reply->url().fileName()).completeSuffix();
	auto fileName = QStringLiteral("img_%1.%2")
					.arg(index, 3, 10, QLatin1Char('0'))
					.arg(suffix);
	QFile imageFile(_tmpDir->filePath(fileName));
	if(!imageFile.open(QIODevice::WriteOnly)) {
		qCritical() << "Failed to open file" << imageFile.fileName()
					<< "for writing with error:" << imageFile.errorString();
		qApp->exit(EXIT_FAILURE);
		return;
	}

	imageFile.write(reply->readAll());
	imageFile.close();
	reply->deleteLater();

	qDebug() << "Successfully downloaded page" << index;
	testDone();
}

void Downloader::testDone()
{
	if(++_progCnt == _pages.size()) {
		qInfo() << "Download successfull!";
		emit downloadComplete(_tmpDir);
	}
}
