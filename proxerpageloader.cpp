#include "proxerpageloader.h"
#include <QDialogButtonBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <dialogmaster.h>

ProxerPageLoader::ProxerPageLoader(QWidget *parent) :
	QDialog(parent),
	_view(new QWebEngineView(this)),
	_loginDone(false),
	_id(-1),
	_next(0),
	_max(0)
{
	guiSetup();
	connect(_view, &QWebEngineView::loadFinished,
			this, &ProxerPageLoader::pageLoadDone);
}

void ProxerPageLoader::startLoading(int id, int firstChapter, int lastChapter)
{
	_id = id;
	_next = firstChapter;
	_max = lastChapter;

	downloadNext();
}

void ProxerPageLoader::downloadNext()
{
	if(!_loginDone) {
		QUrl loginUrl(QStringLiteral("https://proxer.me/login"));
		_view->load(loginUrl);
		open();
		DialogMaster::information(this, tr("Please log in to be able to crawl mangas. Press <ok> once your done."));
		return;
	}

	if(_next > _max) {
		//TODO complete
		return;
	}

	QUrl chapterUrl(QStringLiteral("https://proxer.me/read/%1/%2/en/1")
					.arg(_id)
					.arg(_next));
	_view->load(chapterUrl);
	emit updateProgress(_next, 0, tr("Loading chapter meta-data…"));
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

		if(!_loginDone) //do nothing wait for use to accept
			return;

		//DEBUG
		open();
		emit updateProgress(_next, 0, tr("Successfully downloaded meta-data"));
	} catch (QString &s) {
		if(isVisible())
			close();
		emit updateProgress(_next, 0, s, true);
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

	auto btnBox = new QDialogButtonBox(this);
	btnBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(btnBox, &QDialogButtonBox::accepted,
			this, &ProxerPageLoader::loginDone);
	connect(btnBox, &QDialogButtonBox::rejected,
			this, &ProxerPageLoader::reject);
	subLayout->addWidget(btnBox);

	DialogMaster::masterDialog(this);

}
