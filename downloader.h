#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QQueue>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QTemporaryDir>

class Downloader : public QObject
{
	Q_OBJECT

public:
	explicit Downloader(QObject *parent = nullptr);

	bool setBaseUrl(const QString &url);
	bool parsePages(const QStringList &pages);

signals:
	void downloadComplete(const QSharedPointer<QTemporaryDir> &downloadDir);

private slots:
	void start();

	void replyDone();
	void testDone();

private:
	QString _baseUrl;
	QStringList _pages;

	QNetworkAccessManager *_nam;
	QSharedPointer<QTemporaryDir> _tmpDir;
	int _progCnt;
};

#endif // DOWNLOADER_H
