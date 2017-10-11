TEMPLATE = app

QT += network gui widgets webenginecore webenginewidgets

CONFIG += c++11

TARGET = mangaconv
VERSION = 2.0.0

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
DEFINES += "DISPLAY_NAME=\"\\\"Manga-Convert\\\"\""
DEFINES += "BUNDLE=\"\\\"de.skycoder42\\\"\""

DEFINES += QT_DEPRECATED_WARNINGS QT_ASCII_CAST_WARNINGS

HEADERS += \
	downloader.h \
	pdfcreator.h \
	mainwindow.h \
	proxerpageloader.h

SOURCES += main.cpp \
	downloader.cpp \
	pdfcreator.cpp \
	mainwindow.cpp \
	proxerpageloader.cpp

FORMS += \
	mainwindow.ui

DISTFILES += README.md \
	mangaconv.svg \
	mangaconv.desktop \
	qpmx.json

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)):include($$OUT_PWD/qpmx_generated.pri)
else: error(qpmx initialization failed. Check the compilation log for details.)

RESOURCES += \
    mangaconv.qrc
