#include "apphandler.h"
#include "crypto.h"
#include "profileviewer.h"
#include "accesscontrol.h"
#include "controlpanel.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Before all -- read database.
    AccessControl::getInstance().dbRead();

    AppHandler *ah = new AppHandler();
    ah->startLogin();

    //ControlPanel *cp = new ControlPanel;
    //cp->show();
    
    return a.exec();
}
