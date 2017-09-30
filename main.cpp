#include <QCoreApplication>
#include <QDebug>

#include "downloader.h"
#include "pdfcreator.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QCoreApplication::setApplicationName(QStringLiteral(TARGET));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QCoreApplication::setOrganizationName(QStringLiteral(COMPANY));
	QCoreApplication::setOrganizationDomain(QStringLiteral(BUNDLE));

	Downloader loader;
	PdfCreator creator;

	QObject::connect(&loader, &Downloader::downloadComplete,
					 &creator, &PdfCreator::startConversion);

	QStringList rArgs = QCoreApplication::arguments();
	rArgs.removeFirst();
	if(rArgs.size() != 2) {
		qCritical() << "Usage: mangaconv <title> <input-file>";
		return EXIT_FAILURE;
	}
	creator.setTitle(rArgs.takeFirst());
	if(!loader.readFile(rArgs.takeLast()))
		return EXIT_FAILURE;

	QMetaObject::invokeMethod(&loader, "start", Qt::QueuedConnection);
	return a.exec();
}
