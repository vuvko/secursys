#include "crypto.h"

#include <QDebug>

Crypto::Crypto(const QString &dirPath)
{
    lib.setFileName(dirPath);
    hash_256_func = (hash_func)lib.resolve("hash_256");
    hash_512_func = (hash_func)lib.resolve("hash_512");
}

QString
Crypto::hash_256(const QString &msg)
{
    char *out = (char *)calloc(32, sizeof(*out));
    hash_256_func(msg.toStdString().c_str(), msg.length(), out);
    return QString(out);
}

QString
Crypto::hash_512(const QString &msg)
{
    char *out = (char *)calloc(64, sizeof(*out));
    hash_512_func(msg.toStdString().c_str(), msg.length(), out);
    return QString(out);
}
