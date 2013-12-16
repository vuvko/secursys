#ifndef CRYPTO_H
#define CRYPTO_H

#include <QLibrary>
#include <QFunctionPointer>

class Crypto
{
public:
    Crypto(const QString &dirPath);

    QString hash_256(const QString &msg);
    QString hash_512(const QString &msg);

private:
    typedef void (*hash_func) (const char *, long long, char *);

    QLibrary lib;
    hash_func hash_256_func;
    hash_func hash_512_func;
};

#endif // CRYPTO_H
