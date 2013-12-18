#include "apphandler.h"
#include "crypto.h"
#include "profileviewer.h"
#include "logger.h"

#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppHandler *ah = new AppHandler();
    ah->startLogin();
    //ah->startFS();

    Logger &log = Logger::getInstance();
    log << "New item added to list." << Logger::ENDL;
    log << "With\nMore\nLines" << Logger::ENDL;

    qDebug() << log.unload();

    //ProfileViewer *pv = new ProfileViewer;
    //pv->show();
    
    return a.exec();
}
