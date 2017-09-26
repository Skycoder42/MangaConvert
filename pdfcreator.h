#ifndef PDFCREATOR_H
#define PDFCREATOR_H

#include <QObject>
#include <QSharedPointer>
#include <QTemporaryDir>

class PdfCreator : public QObject
{
	Q_OBJECT

public:
	explicit PdfCreator(QObject *parent = nullptr);

	void setTitle(const QString &title);

public slots:
	void startConversion(const QSharedPointer<QTemporaryDir> &dir);

private:
	QString _title;
};

#endif // PDFCREATOR_H
