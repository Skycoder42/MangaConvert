#include "downloader.h"

#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>
#include <QNetworkReply>
#include <QFile>

Downloader::Downloader(QObject *parent) :
	QObject(parent),
	_input(),
	_nam(new QNetworkAccessManager(this)),
	_tmpDir(new QTemporaryDir()),
	_progCnt(0)
{
	_nam->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}

bool Downloader::readFile(const QString &file)
{
	QFile inputFile(file);
	if(!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qCritical() << "Invalid input file:" << inputFile.errorString();
		return false;
	}

	QTextStream stream(&inputFile);
	while(!stream.atEnd()) {
		auto line = stream.readLine().trimmed().split(QLatin1Char(' '));
		if(line.size() < 2) {
			qCritical() << "Invalid input:" << line.join(QLatin1Char(' '));
			return false;
		}

		auto url = line.takeFirst();
		if(!QUrl(url.arg(QStringLiteral("test"))).isValid()) {
			qCritical() << "Invalid base url:" << url;
			return false;
		}

		auto pages = parsePages(line);
		if(pages.isEmpty())
			return false;

		_input.append({url, pages});
	}

	return true;
}

void Downloader::start()
{
	auto pageCounter = 0;
	foreach(auto input, _input) {
		if(input.second.isEmpty()) {
			qWarning() << "No pages specified. Doing nothing";
			qApp->quit();
			return;
		}

		for(auto i = 0; i < input.second.size(); i++) {
			QUrl url = input.first.arg(input.second[i]);
			if(!url.isValid()) {
				qCritical() << "Invalid generated page url:" << input.first.arg(input.second[i]);
				qApp->exit(EXIT_FAILURE);
				return;
			}

			_progCnt++;
			QNetworkRequest request(url);
			auto reply = _nam->get(request);
			reply->setProperty("index", pageCounter + i);
			connect(reply, &QNetworkReply::finished,
					this, &Downloader::replyDone,
					Qt::QueuedConnection);
		}
		pageCounter += input.second.size();
	}
}

void Downloader::replyDone()
{
	auto reply = qobject_cast<QNetworkReply*>(sender());
	if(!reply)
		return;

	if(reply->error() != QNetworkReply::NoError) {
		qCritical() << "Failed to download image file" << reply->url()
					<< "with error:" << reply->errorString();
		qApp->exit(EXIT_FAILURE);
		return;
	}

	auto index = reply->property("index").toInt();
	auto suffix = QFileInfo(reply->url().fileName()).completeSuffix();
	auto fileName = QStringLiteral("img_%1.%2")
					.arg(index, 3, 10, QLatin1Char('0'))
					.arg(suffix);
	QFile imageFile(_tmpDir->filePath(fileName));
	if(!imageFile.open(QIODevice::WriteOnly)) {
		qCritical() << "Failed to open file" << imageFile.fileName()
					<< "for writing with error:" << imageFile.errorString();
		qApp->exit(EXIT_FAILURE);
		return;
	}

	imageFile.write(reply->readAll());
	imageFile.close();
	reply->deleteLater();

	qDebug() << "Successfully downloaded page" << index;
	testDone();
}

void Downloader::testDone()
{
	if(--_progCnt == 0) {
		qInfo() << "Download successfull!";
		emit downloadComplete(_tmpDir);
	}
}

QStringList Downloader::parsePages(const QStringList &pages)
{
	QStringList res;
	QRegularExpression regex(QStringLiteral(R"__(^(\d+)-(\d+)$)__"));
	regex.optimize();
	foreach(auto page, pages) {
		auto match = regex.match(page);
		if(match.hasMatch()) {
			auto first = match.captured(1);
			auto second = match.captured(2);
			auto width = qMax(first.size(), second.size());

			bool ok;
			auto i = first.toInt(&ok);
			if(!ok) {
				qCritical() << "Invalid argument:" << page;
				return {};
			}
			auto total = second.toInt(&ok);
			if(!ok) {
				qCritical() << "Invalid argument:" << page;
				return {};
			}
			for(; i <= total; i++)
				res.append(QStringLiteral("%1").arg(i, width, 10, QLatin1Char('0')));
		} else
			res.append(page);
	}

	return res;
}
