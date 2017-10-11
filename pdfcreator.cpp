#include "pdfcreator.h"

#include <QStandardPaths>
#include <QGuiApplication>
#include <QDir>

PdfCreator::PdfCreator(QObject *parent) :
	QObject(parent),
	_title(),
	_merge(false),
	_writer(nullptr),
	_painter(nullptr)
{}

void PdfCreator::setMerged(bool merge)
{
	_merge = merge;
}

void PdfCreator::setTitle(const QString &title)
{
	_title = title;
	if(_writer)
		_writer->setTitle(title);
}

void PdfCreator::startConversion(int chapter, const QList<QImage> &images)
{
	try {
		if(_title.isEmpty())
			_title = tr("Unnamed");
		auto name = _title;
		if(_merge)
			name = tr("%1.pdf").arg(name);
		else {
			name = tr("%1 - Chapter %L2.pdf")
				   .arg(name)
				   .arg(chapter);
		}

		if(!_writer) {
			Q_ASSERT(!_painter);

			QDir outDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
			_writer.reset(new QPdfWriter(outDir.absoluteFilePath(name)));
			_writer->setCreator(QGuiApplication::applicationDisplayName());
			_writer->setTitle(_title);
			_writer->setResolution(200);//TODO test

			_painter.reset(new QPainter());
		} else
			Q_ASSERT(_painter);

		foreach (auto img, images) {
			QSizeF imgSize;
			imgSize.setWidth(img.size().width() / (double)_writer->logicalDpiX());
			imgSize.setHeight(img.size().height() / (double)_writer->logicalDpiY());
			_writer->setPageSize(QPageSize(imgSize, QPageSize::Inch, QStringLiteral("image"), QPageSize::ExactMatch));
			if(!_writer->setPageMargins(QMarginsF()))
				throw tr("Failed to set page margins to 0");

			if(_painter->isActive()) {
				if(!_writer->newPage())
					throw tr("Failed to start new PDF page");
			} else {
				if(!_painter->begin(_writer.data()))
					throw tr("Failed to start writing first PDF page");
			}

			_painter->drawImage(QRect({0,0}, img.size()), img);
		}

		if(!_merge) {
			if(!_painter->end())
				throw tr("Failed to finish PDF file");
			_painter.reset();
			_writer.reset();
		}

		emit completed();
	} catch(QString &s) {
		emit updateProgress(chapter, s, true);
	}
}

void PdfCreator::finalize()
{
	if(_merge && _writer) {
		if(!_painter->end())
			throw tr("Failed to finish PDF file");
		_painter.reset();
		_writer.reset();
	}
}
