#include "proxerpageloader.h"
#include <QProgressBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <dialogmaster.h>

ProxerPageLoader::ProxerPageLoader(QWidget *parent) :
	QDialog(parent),
	_view(new QWebEngineView(this)),
	_btnBox(new QDialogButtonBox(this)),
	_loginDone(false),
	_id(-1),
	_current(0),
	_max(0)
{
	guiSetup();
	connect(_view, &QWebEngineView::loadStarted, this, [this](){
		_btnBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	});
	connect(_view, &QWebEngineView::loadFinished,
			this, &ProxerPageLoader::pageLoadDone);
	connect(this, &ProxerPageLoader::rejected, [this](){
		emit updateProgress(_current, tr("Login abort by user!"), true);
	});
}

void ProxerPageLoader::init(int id, int firstChapter, int lastChapter)
{
	_id = id;
	_current = firstChapter;
	_max = lastChapter;
}

void ProxerPageLoader::downloadNext()
{
	if(!_loginDone) {
		QUrl loginUrl(QStringLiteral("https://proxer.me/login"));
		_view->load(loginUrl);
		emit updateProgress(_current, tr("Loading login page…"));
		open();
		DialogMaster::information(this, tr("Please log in to be able to crawl mangas. Press <ok> once your done."));
		return;
	}

	if(_current > _max) {
		emit allChaptersLoaded();
		return;
	}

	QUrl chapterUrl(QStringLiteral("https://proxer.me/read/%1/%2/en/1")
					.arg(_id)
					.arg(_current));
	_view->load(chapterUrl);
	emit updateProgress(_current, tr("Loading chapter meta-data…"));
}

void ProxerPageLoader::loginDone()
{
	if(_loginDone)
		return;
	_loginDone = true;
	accept();
	downloadNext();
}

void ProxerPageLoader::pageLoadDone(bool success)
{
	try {
		if(!success)
			throw tr("Failed to open proxer.me login page.");

		if(!_loginDone){
			//do nothing wait for use to accept
			emit updateProgress(_current, tr("Page loaded. Waiting for user to log in…"));
			_btnBox->button(QDialogButtonBox::Ok)->setEnabled(true);
			return;
		}

		//extract the variables
		auto script = QStringLiteral("var callback_arr = {};"
									 "callback_arr[\"title\"] = document.getElementById(\"breadcrumb\").childNodes[1].textContent;"
									 "callback_arr[\"pages\"] = pages;"
									 "callback_arr[\"nextChapter\"] = nextChapter;"
									 "callback_arr[\"serverurl\"] = serverurl;"
									 "callback_arr");
		_view->page()->runJavaScript(script, [this](const QVariant &result){
			jsExtract(result.toMap());
		});
		emit updateProgress(_current, tr("Running extraction script…"));
	} catch (QString &s) {
		if(isVisible())
			close();
		emit updateProgress(_current, s, true);
	}
}

void ProxerPageLoader::jsExtract(const QVariantMap &map)
{
	if(map.isEmpty())
		emit updateProgress(_current, tr("Failed to extract meta-data from loaded page!"), true);
	else {
		QList<QPair<QUrl, QSize>> images;

		auto title = map.value(QStringLiteral("title")).toString().trimmed();
		if(!title.isEmpty())
			emit titleDetected(title);

		QUrl baseUrl(map.value(QStringLiteral("serverurl")).toString());
		baseUrl.setScheme(QStringLiteral("https"));

		auto pages = map.value(QStringLiteral("pages")).toList();
		foreach(auto page, pages) {
			auto pageArray = page.toList();

			auto imgUrl = baseUrl;
			imgUrl.setPath(imgUrl.path() + pageArray[0].toString());
			QSize size;
			size.setHeight(pageArray[1].toInt());
			size.setWidth(pageArray[2].toInt());
			images.append({imgUrl, size});
		}

		static QRegularExpression regex(QStringLiteral(R"__(chapter\/\d+\/(\d+)\/)__"), QRegularExpression::OptimizeOnFirstUsageOption);
		auto match = regex.match(map.value(QStringLiteral("nextChapter")).toString());
		auto next = _current + 1;
		if(match.hasMatch())
			next = match.captured(1).toInt();

		emit updateProgress(_current, tr("Successfully downloaded meta-data and extracted manga pages"));
		emit imagesLoaded(_current, images);
		_current = next;
	}
}

void ProxerPageLoader::guiSetup()
{
	auto layout = new QVBoxLayout(this);
	layout->addWidget(_view);
	setLayout(layout);
	auto m = layout->contentsMargins();
	layout->setContentsMargins({});
	layout->setSpacing(0);

	auto subLayout = new QHBoxLayout(this);
	subLayout->setContentsMargins(m);
	layout->addLayout(subLayout);

	auto bar = new QProgressBar(this);
	bar->setRange(0, 100);
	connect(_view, &QWebEngineView::loadProgress, this, [bar](int progress){
		bar->setValue(progress);
		bar->setVisible(progress != 100 && progress != 0);
	});
	subLayout->addWidget(bar);

	_btnBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(_btnBox, &QDialogButtonBox::accepted,
			this, &ProxerPageLoader::loginDone);
	connect(_btnBox, &QDialogButtonBox::rejected,
			this, &ProxerPageLoader::reject);
	subLayout->addWidget(_btnBox);

	DialogMaster::masterDialog(this);

}
