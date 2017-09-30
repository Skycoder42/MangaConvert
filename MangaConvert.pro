TEMPLATE = app

QT += network
QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = mangaconv
VERSION = 1.1.0

DEFINES += "TARGET=\\\"$$TARGET\\\""
DEFINES += "VERSION=\\\"$$VERSION\\\""
DEFINES += "COMPANY=\"\\\"Skycoder42\\\"\""
DEFINES += "BUNDLE=\"\\\"de.skycoder42\\\"\""

DEFINES += QT_DEPRECATED_WARNINGS QT_ASCII_CAST_WARNINGS

HEADERS += \
	downloader.h \
	pdfcreator.h

SOURCES += main.cpp \
	downloader.cpp \
	pdfcreator.cpp

target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
