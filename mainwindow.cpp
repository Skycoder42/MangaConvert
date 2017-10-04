#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <dialogmaster.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	_ui(new Ui::MainWindow),
	_taskbarControl(new QTaskbarControl(this)),
	_pageLoader(new ProxerPageLoader(this)),
	_downloader(new Downloader(this)),
	_pdfCreator(new PdfCreator(this)),
	_running(false),
	_chapterOffset(0),
	_chapterRange(1.0),
	_itemCache()
{
	_ui->setupUi(this);

	_ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	_ui->treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);

	_taskbarControl->setAttribute(QTaskbarControl::LinuxDesktopFile, QStringLiteral("DigiaQt-qtcreator-community.desktop"));//TODO debug
	_taskbarControl->setProgressVisible(false);
	_taskbarControl->setCounterVisible(false);

	connect(_pageLoader, &ProxerPageLoader::updateProgress,
			this, &MainWindow::updateProgress);
	connect(_pageLoader, &ProxerPageLoader::imagesLoaded,
			_downloader, &Downloader::downloadChapter);
	connect(_pageLoader, &ProxerPageLoader::titleDetected,
			_pdfCreator, &PdfCreator::setTitle);
	connect(_pageLoader, &ProxerPageLoader::allChaptersLoaded,
			this, &MainWindow::complete);

	connect(_downloader, &Downloader::updateProgress,
			this, &MainWindow::updateProgress);
	connect(_downloader, &Downloader::downloadComplete,
			_pdfCreator, &PdfCreator::startConversion);

	connect(_pdfCreator, &PdfCreator::updateProgress,
			this, &MainWindow::updateProgress);
	connect(_pdfCreator, &PdfCreator::completed,
			_pageLoader, &ProxerPageLoader::scheduleNext);
}

MainWindow::~MainWindow()
{
	_taskbarControl->setProgressVisible(false);
	_taskbarControl->setCounterVisible(false);
	delete _ui;
}

void MainWindow::updateProgress(int chapter, const QString &log, bool error)
{
	_ui->progressBar->setValue(chapter);
	_taskbarControl->setProgress((chapter - _chapterOffset) / _chapterRange);
	_taskbarControl->setCounter(chapter);

	auto item = new QTreeWidgetItem(_ui->treeWidget);
	item->setText(0, QLocale().toString(chapter));
	item->setText(1, log);
	_ui->treeWidget->scrollToBottom();

	if(error) {
		qApp->alert(this);
		item->setIcon(2, style()->standardIcon(QStyle::SP_MessageBoxCritical));
		DialogMaster::critical(this,
							   log,
							   tr("Failed to get chapter %L1").arg(chapter));
	}
}

void MainWindow::complete()
{
	_ui->progressBar->setValue(_ui->progressBar->maximum());
	_taskbarControl->setProgress(1.0);

	bool openDownload = true;
	auto setup = DialogMaster::createInformation(tr("Successfully downloaded all chapters!"), this);
	setup.checkString = tr("Open Download folder");
	setup.checked = &openDownload;
	DialogMaster::messageBox(setup);

	if(openDownload)
		QDesktopServices::openUrl(QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)));
}

void MainWindow::on_startButton_clicked()
{
	_running = true;
	_ui->setupBox->setEnabled(false);

	_chapterOffset = _ui->firstChapterSpinBox->value();
	auto max = _ui->lastChapterSpinBox->value() + 1;
	_chapterRange = (double)(max - _chapterOffset);

	_ui->progressBar->setRange(_chapterOffset, max);
	_ui->progressBar->setValue(_chapterOffset);

	_taskbarControl->setProgress(0.0);
	_taskbarControl->setProgressVisible(true);
	_taskbarControl->setCounter(0);
	_taskbarControl->setCounterVisible(true);

	_pageLoader->start(_ui->mangaIDSpinBox->value(), _chapterOffset, max);
}

void MainWindow::on_firstChapterSpinBox_valueChanged(int chapter)
{
	_ui->lastChapterSpinBox->setMinimum(chapter);
}
