#ifndef PDFCREATOR_H
#define PDFCREATOR_H

#include <QObject>
#include <QPainter>
#include <QPdfWriter>

class PdfCreator : public QObject
{
	Q_OBJECT

public:
	explicit PdfCreator(QObject *parent = nullptr);

public slots:
	void setMerged(bool merge);
	void setTitle(const QString &title);
	void startConversion(int chapter, const QList<QImage> &images);
	void finalize();

signals:
	void updateProgress(int chapter, const QString &log, bool error = false);
	void completed();

private:
	QString _title;
	bool _merge;

	QScopedPointer<QPdfWriter> _writer;
	QScopedPointer<QPainter> _painter;
};

#endif // PDFCREATOR_H
