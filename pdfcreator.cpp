#include "pdfcreator.h"

#include <QProcess>
#include <QCoreApplication>
#include <QDebug>

PdfCreator::PdfCreator(QObject *parent) :
	QObject(parent),
	_title()
{}

void PdfCreator::setTitle(const QString &title)
{
	_title = title;
	if(!_title.endsWith(QStringLiteral(".pdf")))
		_title +=  QStringLiteral(".pdf");
}

void PdfCreator::startConversion(const QSharedPointer<QTemporaryDir> &dir)
{
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

	if(proc.exitStatus() != QProcess::NormalExit) {
		qCritical() << "convert process crashed with error:" << proc.errorString();
		qApp->exit(EXIT_FAILURE);
	} else if(proc.exitCode() == EXIT_SUCCESS) {
		if(QFile::rename(dir->filePath(_title), QDir::current().absoluteFilePath(_title)))
			qInfo() << "Succesfully convert to pdf:" << QDir::current().absoluteFilePath(_title);
		else {
			qWarning() << "Failed to move file from temporary directory! Copy it yourself from:" << dir->path();
			dir->setAutoRemove(false);
		}
	} else
		qCritical() << "convert process failed with exit code" << proc.exitCode();
	qApp->exit(proc.exitCode());
}
