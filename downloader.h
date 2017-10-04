#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QQueue>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QTemporaryDir>
#include <QSize>

class Downloader : public QObject
{
	Q_OBJECT

public:
	explicit Downloader(QObject *parent = nullptr);

public slots:
	void downloadChapter(int chapter, const QList<QPair<QUrl, QSize>> &images);

signals:
	void updateProgress(int chapter, const QString &log, bool error = false);
	void downloadComplete(int chapter, const QSharedPointer<QTemporaryDir> &downloadDir);

private slots:
	void replyDone();
	void testDone();

private:
	int _chapter;
	QList<QPair<QUrl, QSize>> _images;

	QNetworkAccessManager *_nam;
	QSharedPointer<QTemporaryDir> _tmpDir;
	int _progressCounter;

	QStringList parsePages(const QStringList &pages);
};

#endif // DOWNLOADER_H
