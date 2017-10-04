#include "downloader.h"

#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QFile>

Downloader::Downloader(QObject *parent) :
	QObject(parent),
	_chapter(0),
	_images(),
	_nam(new QNetworkAccessManager(this)),
	_tmpDir(),
	_progressCounter(0)
{
	_nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

void Downloader::downloadChapter(int chapter, const QList<QPair<QUrl, QSize> > &images)
{
	Q_ASSERT(!_tmpDir);

	_chapter = chapter;
	_images = images;
	_tmpDir.reset(new QTemporaryDir());

	_progressCounter = images.size();
	auto pageCounter = 0;
	foreach(auto input, _images) {
		QUrl url = input.first;
		QNetworkRequest request(url);
		auto reply = _nam->get(request);
		reply->setProperty("index", pageCounter++);
		connect(reply, &QNetworkReply::finished,
				this, &Downloader::replyDone,
				Qt::QueuedConnection);
	}

	emit updateProgress(_chapter, tr("Started download of %n image(s)", "", images.size()));
}

void Downloader::replyDone()
{
	auto reply = qobject_cast<QNetworkReply*>(sender());
	if(!reply)
		return;
	auto index = reply->property("index").toInt();

	try {
		if(reply->error() != QNetworkReply::NoError)
			throw tr("Network request failed with error: %1").arg(reply->errorString());

		auto suffix = QFileInfo(reply->url().fileName()).suffix();
		auto fileName = QStringLiteral("img_%1.%2")
						.arg(index, 4, 10, QLatin1Char('0'))
						.arg(suffix);
		QFile imageFile(_tmpDir->filePath(fileName));
		if(!imageFile.open(QIODevice::WriteOnly)) {
			throw tr("Failed to open file %1 or writing with error: %2")
					.arg(imageFile.fileName())
					.arg(imageFile.errorString());
		}

		imageFile.write(reply->readAll());
		imageFile.close();
		reply->deleteLater();

		testDone();
	} catch (QString &s) {
		emit updateProgress(_chapter,
							tr("Failure on image no. %1 (url: %2). Error:\n%3")
							.arg(index)
							.arg(reply->url().toString())
							.arg(s));
	}
}

void Downloader::testDone()
{
	if(--_progressCounter == 0) {
		emit updateProgress(_chapter, tr("Downloaded all images for chapter"));
		emit downloadComplete(_chapter, _tmpDir);
		_tmpDir.reset();
	} else
		emit updateProgress(_chapter, tr("Image downloaded. %n image(s) remaining", "", _progressCounter));
}

QStringList Downloader::parsePages(const QStringList &pages)
{
	QStringList res;
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
				return {};
			}
			auto total = second.toInt(&ok);
			if(!ok) {
				qCritical() << "Invalid argument:" << page;
				return {};
			}
			for(; i <= total; i++)
				res.append(QStringLiteral("%1").arg(i, width, 10, QLatin1Char('0')));
		} else
			res.append(page);
	}

	return res;
}
