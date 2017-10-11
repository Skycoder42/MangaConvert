#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QSize>
#include <QImage>

class Downloader : public QObject
{
	Q_OBJECT

public:
	explicit Downloader(QObject *parent = nullptr);

public slots:
	void downloadChapter(int chapter, const QList<QPair<QUrl, QSize>> &images);

signals:
	void updateProgress(int chapter, const QString &log, bool error = false);
	void downloadComplete(int chapter, const QList<QImage> &images);

	void cancelAll();

private slots:
	void replyDone();
	void testDone();

private:
	int _chapter;
	QList<QPair<QUrl, QSize>> _images;

	QNetworkAccessManager *_nam;
	QVector<QImage> _imageResults;
	int _progressCounter;
};

#endif // DOWNLOADER_H
