#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <qtaskbarcontrol.h>

#include "proxerpageloader.h"

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
	void updateProgress(int chapter, int page, const QString &log, bool error);

private slots:
	void on_startButton_clicked();
	void on_firstChapterSpinBox_valueChanged(int chapter);

private:
	Ui::MainWindow *_ui;
	QTaskbarControl *_taskbarControl;

	ProxerPageLoader *_loader;

	bool _running;
	double _chapterDelta;
	QHash<QPair<int, int>, QTreeWidgetItem*> _itemCache;

	void start();
	void stop();
};

#endif // MAINWINDOW_H
