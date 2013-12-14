#-------------------------------------------------
#
# Project created by QtCreator 2013-12-14T14:08:20
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FileViewer
TEMPLATE = app


SOURCES += main.cpp\
        fileviewer.cpp

HEADERS  += fileviewer.h

OTHER_FILES += \
    icons/secret.png \
    icons/prev.png \
    icons/next.png \
    icons/home.png \
    icons/find.png \
    icons/file.png \
    icons/exec.png \
    icons/dir.png \
    icons/user_info.png \
    icons/user.png \
    icons/ok.png \
    icons/dir_empty.png \
    icons/delete.png \
    icons/secret_delete.png \
    icons/dir_delete.png \
    icons/up.png

RESOURCES += \
    icons.qrc
