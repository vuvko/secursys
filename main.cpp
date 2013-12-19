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

    LOG << "New item added to list." << ENDL;
    LOG << "With\nMore\nLines" << ENDL;

    qDebug() << LOG.read();

    //ProfileViewer *pv = new ProfileViewer;
    //pv->show();
    
    return a.exec();
}
