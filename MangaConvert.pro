TEMPLATE = app

QT += network gui widgets
linux: QT += dbus #TODO remove after qpmx update

CONFIG += c++11

TARGET = mangaconv
VERSION = 2.0.0

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
DEFINES += "BUNDLE=\"\\\"de.skycoder42\\\"\""

DEFINES += QT_DEPRECATED_WARNINGS QT_ASCII_CAST_WARNINGS

HEADERS += \
	downloader.h \
	pdfcreator.h \
	mainwindow.h \
    pageloader.h

SOURCES += main.cpp \
	downloader.cpp \
	pdfcreator.cpp \
	mainwindow.cpp \
    pageloader.cpp

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target

FORMS += \
	mainwindow.ui

system(qpmx -d $$shell_quote($$_PRO_FILE_PWD_) --qmake-run init $$QPMX_EXTRA_OPTIONS $$shell_quote($$QMAKE_QMAKE) $$shell_quote($$OUT_PWD)):include($$OUT_PWD/qpmx_generated.pri)
else: error(qpmx initialization failed. Check the compilation log for details.)
