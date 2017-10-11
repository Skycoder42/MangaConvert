#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qtaskbarcontrol.h>

#include "proxerpageloader.h"
#include "downloader.h"
#include "pdfcreator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

public slots:
	void updateProgress(int chapter, const QString &log, bool error);

private slots:
	void complete();

	void on_startButton_clicked();
	void on_firstChapterSpinBox_valueChanged(int chapter);

private:
	Ui::MainWindow *_ui;
	QTaskbarControl *_taskbarControl;

	ProxerPageLoader *_pageLoader;
	Downloader *_downloader;
	PdfCreator *_pdfCreator;

	bool _running;
	int _chapterOffset;
	double _chapterRange;
};

#endif // MAINWINDOW_H
