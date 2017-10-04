#include "pdfcreator.h"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>
#include <QStandardPaths>

PdfCreator::PdfCreator(QObject *parent) :
	QObject(parent),
	_title()
{}

void PdfCreator::setTitle(const QString &title)
{
	_title = title;
}

void PdfCreator::startConversion(int chapter, const QSharedPointer<QTemporaryDir> &dir)
{
	try {
		if(_title.isEmpty())
			_title = tr("Unnamed");
		_title = tr("%1 - Chapter %L2.pdf")
				 .arg(_title)
				 .arg(chapter);

		QDir outDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));

		QProcess proc;
		proc.setProgram(QStringLiteral("convert"));
		proc.setArguments({
							  QStringLiteral("img_*"),
							  _title
						  });
		proc.setWorkingDirectory(dir->path());
		proc.setProcessChannelMode(QProcess::ForwardedChannels);
		proc.setInputChannelMode(QProcess::ForwardedInputChannel);
		proc.start();
		proc.waitForFinished(-1);

		if(proc.exitStatus() != QProcess::NormalExit)
			throw tr("convert process crashed with error: %1").arg(proc.errorString());

		if(proc.exitCode() == EXIT_SUCCESS) {
			if(QFile::rename(dir->filePath(_title), outDir.absoluteFilePath(_title)))
				emit updateProgress(chapter, tr("Successfully converted and moved pdf to downloads folder"));
			else {
				dir->setAutoRemove(false);
				throw tr("Failed to move file from temporary directory! Copy it yourself from: %1").arg(dir->path());
			}
		} else
			throw tr("convert process failed with exit code: %1").arg(proc.exitCode());

		emit completed();
	} catch(QString &s) {
		emit updateProgress(chapter, s, true);
	}
}
