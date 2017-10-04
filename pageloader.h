#ifndef PAGELOADER_H
#define PAGELOADER_H

#include <QNetworkAccessManager>
#include <QObject>

class PageLoader : public QObject
{
	Q_OBJECT

public:
	explicit PageLoader(QObject *parent = nullptr);

public slots:
	void startLoading(int id, int firstChapter, int lastChapter);

signals:
	void updateProgress(int chapter, int page, const QString &log, bool error = false);

private slots:
	void downloadNext();

	void pageReplyDone();

private:
	QNetworkAccessManager *_nam;

	int _id;
	int _next;
	int _max;
};

#endif // PAGELOADER_H
