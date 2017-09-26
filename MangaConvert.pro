TEMPLATE = app

QT += network
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = mangaconv

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
	downloader.h \
    pdfcreator.h

SOURCES += main.cpp \
	downloader.cpp \
    pdfcreator.cpp
