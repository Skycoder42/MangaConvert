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

public slots:
	void setTitle(const QString &title);
	void startConversion(int chapter, const QSharedPointer<QTemporaryDir> &dir);

signals:
	void updateProgress(int chapter, const QString &log, bool error = false);
	void completed();

private:
	QString _title;
};

#endif // PDFCREATOR_H
