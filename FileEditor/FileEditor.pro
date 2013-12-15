QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileEditor
TEMPLATE = lib

RESOURCES += \
    ../icons.qrc

HEADERS += \
    fileeditor.h

SOURCES += \
    fileeditor.cpp
