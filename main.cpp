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
	if(rArgs.size() < 3) {
		qCritical() << "Usage: mangaconv <title> <base-url> <pages> ...";
		return EXIT_FAILURE;
	}
	creator.setTitle(rArgs.takeFirst());
	if(!loader.setBaseUrl(rArgs.takeFirst()))
		return EXIT_FAILURE;
	if(!loader.parsePages(rArgs))
		return EXIT_FAILURE;

	QMetaObject::invokeMethod(&loader, "start", Qt::QueuedConnection);
	return a.exec();
}
