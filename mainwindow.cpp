#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <dialogmaster.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow),
	_taskbarControl(new QTaskbarControl(this)),
	_loader(new PageLoader(this)),
	_running(false),
	_chapterDelta(1.0),
	_itemCache()
{
	_ui->setupUi(this);

	auto pal = _ui->lcdNumber->palette();
	pal.setColor(QPalette::WindowText, QApplication::palette().color(QPalette::Highlight));
	_ui->lcdNumber->setPalette(pal);

	_ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	_ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	_ui->treeWidget->header()->setSectionResizeMode(2, QHeaderView::Interactive);

	_taskbarControl->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("DigiaQt-qtcreator-community.desktop"));//TODO debug
	_taskbarControl->setProgressVisible(false);
	_taskbarControl->setCounterVisible(false);

	connect(_loader, &PageLoader::updateProgress,
			this, &MainWindow::updateProgress);
}

MainWindow::~MainWindow()
{
	_taskbarControl->setProgressVisible(false);
	_taskbarControl->setCounterVisible(false);
	delete _ui;
}

void MainWindow::updateProgress(int chapter, int page, const QString &log, bool error)
{
	_ui->progressBar->setValue(chapter);
	_taskbarControl->setProgress(chapter / _chapterDelta);

	_ui->lcdNumber->display(page);
	_taskbarControl->setCounter(page);

	auto item = _itemCache.value({chapter, page});
	if(!item) {
		QLocale locale;
		item = new QTreeWidgetItem(_ui->treeWidget);
		item->setText(0, locale.toString(chapter));
		item->setText(1, locale.toString(page));
		_itemCache.insert({chapter, page}, item);
	}
	item->setText(2, log);

	if(error) {
		qApp->alert(this);
		item->setIcon(2, style()->standardIcon(QStyle::SP_MessageBoxCritical));
		DialogMaster::critical(this,
							   log,
							   tr("Failed to get %1 of chapter %L2")
							   .arg(page == 0 ?
										tr("meta-data") :
										tr("page %L1").arg(page))
							   .arg(chapter));
		stop();
	}
}

void MainWindow::on_startButton_clicked()
{
	if(!_running)
		start();
	else
		Q_UNIMPLEMENTED();
}

void MainWindow::start()
{
	_running = true;

	_ui->mangaIDSpinBox->setEnabled(false);
	_ui->firstChapterSpinBox->setEnabled(false);
	_ui->lastChapterSpinBox->setEnabled(false);
	_ui->startButton->setText(tr("Cancel"));

	auto min = _ui->firstChapterSpinBox->value();
	auto max = _ui->lastChapterSpinBox->value();
	_chapterDelta = (double)(max - min);

	_ui->progressBar->setRange(min, max);
	_ui->progressBar->setValue(min);
	_ui->lcdNumber->display(0);

	_taskbarControl->setProgress(0.0);
	_taskbarControl->setProgressVisible(true);
	_taskbarControl->setCounter(0);
	_taskbarControl->setCounterVisible(true);

	_loader->startLoading(_ui->mangaIDSpinBox->value(), min, max);
}

void MainWindow::stop()
{
	Q_UNIMPLEMENTED();
}

void MainWindow::on_firstChapterSpinBox_valueChanged(int chapter)
{
	_ui->lastChapterSpinBox->setMinimum(chapter);
}
