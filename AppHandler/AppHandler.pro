QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AppHandler
TEMPLATE = lib

HEADERS += \
    apphandler.h

SOURCES += \
    apphandler.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../FSViewer/release/ -lFSViewer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../FSViewer/debug/ -lFSViewer
else:unix:!macx: LIBS += -L$$OUT_PWD/../FSViewer/ -lFSViewer

INCLUDEPATH += $$PWD/../FSViewer
DEPENDPATH += $$PWD/../FSViewer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../FileEditor/release/ -lFileEditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../FileEditor/debug/ -lFileEditor
else:unix:!macx: LIBS += -L$$OUT_PWD/../FileEditor/ -lFileEditor

INCLUDEPATH += $$PWD/../FileEditor
DEPENDPATH += $$PWD/../FileEditor
