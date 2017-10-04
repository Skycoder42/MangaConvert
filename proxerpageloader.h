#ifndef PROXERPAGELOADER_H
#define PROXERPAGELOADER_H

#include <QDialog>
#include <QWebEngineView>

class ProxerPageLoader : public QDialog
{
	Q_OBJECT

public:
	explicit ProxerPageLoader(QWidget *parent = nullptr);

public slots:
	void startLoading(int id, int firstChapter, int lastChapter);

signals:
	void updateProgress(int chapter, int page, const QString &log, bool error = false);

private slots:
	void downloadNext();
	void loginDone();

	void pageLoadDone(bool success);

private:
	QWebEngineView *_view;
	bool _loginDone;

	int _id;
	int _next;
	int _max;

	void guiSetup();
};

#endif // PROXERPAGELOADER_H
