QT += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = SecurSys
TEMPLATE = app

RESOURCES += \
    icons.qrc

SOURCES += \
    main.cpp \
    logindialog.cpp \
    logger.cpp \
    fsviewer.cpp \
    fileeditor.cpp \
    apphandler.cpp \
    crypto.cpp

HEADERS += \
    logindialog.h \
    logger.h \
    fsviewer.h \
    fileeditor.h \
    apphandler.h \
    crypto.h

#LIBS += -L$$_PRO_FILE_PWD_ -lAES256

