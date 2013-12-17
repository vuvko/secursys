#include "apphandler.h"
#include "crypto.h"
#include "profileviewer.h"

#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //AppHandler *ah = new AppHandler();
    //ah->startLogin();
    //ah->startFS();

    //ProfileViewer *pv = new ProfileViewer;
    //pv->show();
    Crypto cr("AES256.dll");
    qDebug() << cr.hash_256("Some text is using hash.").toHex().toUpper();
    QByteArray key = cr.generate_next();
    QByteArray msg(1024, 0);
    qDebug() << "Key: " << key.toHex().toUpper();
    qDebug() << "Msg: " << msg.toHex().toUpper();
    qDebug() << "Size: " << msg.size() << ((msg.size() + 1) / 16) * 16;
    QByteArray encr = cr.encrypt(msg, QByteArray(32, 0));
    qDebug() << "Encrypted: " << encr.toHex().toUpper();
    qDebug() << "Decrypted: " << cr.decrypt(QByteArray(1024, 0), key);
    
    return a.exec();
}
