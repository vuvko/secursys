#include "apphandler.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppHandler *ah = new AppHandler();
    //ah->startLogin();
    ah->startFS();
    
    return a.exec();
}
