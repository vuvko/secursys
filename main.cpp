#include "apphandler.h"
#include "crypto.h"
#include "profileviewer.h"

#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppHandler *ah = new AppHandler();
    //ah->startLogin();
    ah->startFS();

    //ProfileViewer *pv = new ProfileViewer;
    //pv->show();
    //Crypto cr("AES256.dll");
    //qDebug() << cr.hash_256("Some text is using hash.").toLocal8Bit().toHex();
    
    return a.exec();
}
