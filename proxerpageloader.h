#ifndef PROXERPAGELOADER_H
#define PROXERPAGELOADER_H

#include <QDialog>
#include <QWebEngineView>
#include <QDialogButtonBox>
#include <QVariant>

class ProxerPageLoader : public QDialog
{
	Q_OBJECT

public:
	explicit ProxerPageLoader(QWidget *parent = nullptr);

public slots:
	void init(int id, int firstChapter, int lastChapter);
	void downloadNext();

signals:
	void titleDetected(const QString &title);
	void updateProgress(int chapter, const QString &log, bool error = false);
	void imagesLoaded(int chapter, const QList<QPair<QUrl, QSize>> &images);

	void allChaptersLoaded();

private slots:
	void loginDone();

	void pageLoadDone(bool success);
	void jsExtract(const QVariantMap &map);

private:
	QWebEngineView *_view;
	QDialogButtonBox *_btnBox;
	bool _loginDone;

	int _id;
	int _current;
	int _max;

	void guiSetup();
};

#endif // PROXERPAGELOADER_H
