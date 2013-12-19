#include "apphandler.h"
#include "crypto.h"
#include "profileviewer.h"
#include "accesscontrol.h"

#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Before all -- read database.
    AccessControl::getInstance().dbRead();

    AppHandler *ah = new AppHandler();
    ah->startLogin();
    //ah->startFS();

    //ProfileViewer *pv = new ProfileViewer;
    //pv->show();
    
    return a.exec();
}
