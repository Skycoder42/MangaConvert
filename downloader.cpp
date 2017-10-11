#include "downloader.h"

#include <QNetworkReply>
#include <QImageReader>

Downloader::Downloader(QObject *parent) :
	QObject(parent),
	_chapter(0),
	_images(),
	_nam(new QNetworkAccessManager(this)),
	_imageResults(),
	_progressCounter(0)
{
	_nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

void Downloader::downloadChapter(int chapter, const QList<QPair<QUrl, QSize> > &images)
{
	_chapter = chapter;
	_images = images;
	_imageResults.fill(QImage(), images.size());

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
		connect(this, &Downloader::cancelAll,
				reply, &QNetworkReply::abort,
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

		QImageReader reader;
		reader.setDevice(reply);
		reader.setQuality(100);
		reader.setScaledSize(_images[index].second);
		_imageResults[index] = reader.read();
		if(_imageResults[index].isNull())
			throw tr("Failed to create image from download with errror: %1").arg(reader.errorString());

		reply->deleteLater();
		testDone();
	} catch (QString &s) {
		emit cancelAll();
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
		emit downloadComplete(_chapter, _imageResults.toList());
		_imageResults.clear();
		_images.clear();
		_chapter = 0;
	} else
		emit updateProgress(_chapter, tr("Image downloaded. %n image(s) remaining", "", _progressCounter));
}
