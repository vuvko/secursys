#include "apphandler.h"
#include "crypto.h"
#include "profileviewer.h"

#include <QApplication>

#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppHandler *ah = new AppHandler();
    ah->startLogin();
    //ah->startFS();

    //ProfileViewer *pv = new ProfileViewer;
    //pv->show();
    Crypto cr("AES256.dll");
    //qDebug() << cr.hash_256("Some text is using hash.").toLocal8Bit().toHex();
    QByteArray key = cr.generate_next();
    QString msg = "Some text without encryption.";
    qDebug() << "Key: " << key.toHex().toUpper();
    qDebug() << "Msg: " << msg;
    qDebug() << "Size: " << msg.size() << msg.size() - msg.size() % 16 + 16;
    QByteArray encr = cr.encrypt(msg.toLocal8Bit(), key);
    qDebug() << "Encrypted: " << encr.toHex().toUpper();
    qDebug() << "Decrypted: " << cr.decrypt(encr, key);
    
    return a.exec();
}
