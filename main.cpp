#include "apphandler.h"
#include "crypto.h"
#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppHandler *ah = new AppHandler();
    ah->startLogin();
    //ah->startFS();
    //Crypto cr("/home/andrey/QT/SecurSys/AES256.dll");
    //qDebug() << cr.hash_256("Some text is using hash.");
    
    return a.exec();
}
