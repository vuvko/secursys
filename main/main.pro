QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = SecurSys
TEMPLATE = app

SOURCES += \
    main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../FSViewer/release/ -lFileViewer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../FSViewer/debug/ -lFileViewer
else:unix:!macx: LIBS += -L$$OUT_PWD/../FSViewer/ -lFSViewer

INCLUDEPATH += $$PWD/../FSViewer
DEPENDPATH += $$PWD/../FSViewer

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../FileEditor/release/ -lFileEditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../FileEditor/debug/ -lFileEditor
else:unix:!macx: LIBS += -L$$OUT_PWD/../FileEditor/ -lFileEditor

INCLUDEPATH += $$PWD/../FileEditor
DEPENDPATH += $$PWD/../FileEditor

HEADERS +=

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../AppHandler/release/ -lAppHandler
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../AppHandler/debug/ -lAppHandler
else:unix:!macx: LIBS += -L$$OUT_PWD/../AppHandler/ -lAppHandler

INCLUDEPATH += $$PWD/../AppHandler
DEPENDPATH += $$PWD/../AppHandler
