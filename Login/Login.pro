QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Login
TEMPLATE = lib

HEADERS += \
    logindialog.h

SOURCES += \
    logindialog.cpp

RESOURCES += \
    ../icons.qrc

