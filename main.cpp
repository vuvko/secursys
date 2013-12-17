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
    QVector<char> key = cr.generate_next();
    QString msg = "Some text without encryption.";
    qDebug() << "Key: " << QString(key.data()).toLocal8Bit().toHex().toUpper();
    qDebug() << "Msg: " << msg;
    qDebug() << "Size: " << msg.size() << msg.size() - msg.size() % 16 + 16;
    QVector<char> encr = cr.encrypt(msg, key);
    qDebug() << "Encrypted: " << QString(encr.data()).toLocal8Bit().toHex().toUpper();
    qDebug() << "Decrypted: " << cr.decrypt(encr, key);
    
    return a.exec();
}
