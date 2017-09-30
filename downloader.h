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

	bool readFile(const QString &file);

signals:
	void downloadComplete(const QSharedPointer<QTemporaryDir> &downloadDir);

private slots:
	void start();

	void replyDone();
	void testDone();

private:
	QList<QPair<QString, QStringList>> _input;

	QNetworkAccessManager *_nam;
	QSharedPointer<QTemporaryDir> _tmpDir;
	int _progCnt;

	QStringList parsePages(const QStringList &pages);
};

#endif // DOWNLOADER_H
