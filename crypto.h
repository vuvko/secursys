#ifndef CRYPTO_H
#define CRYPTO_H

#include <QLibrary>

class Crypto
{
public:
    Crypto(const QString &dirPath);

    QString hash_256(const QString &msg);
    QString hash_512(const QString &msg);

    QString encrypt(const QString &msg, const QList<char> &key);
    QString decrypt(const QString &msg, const QList<char> &key);

private:
    typedef void (*hash_func_t) (const unsigned char *, unsigned long long, unsigned char *);
    typedef int (*encrypt_func_t) (unsigned char *, unsigned long long, unsigned char *, unsigned char *);
    typedef int (*decrypt_func_t) (unsigned char *, unsigned long long, unsigned char *, unsigned char *, unsigned long long *);
    typedef int (*gen_func_t) (unsigned char *, unsigned long long);

    QList<char> generate_next();

    QLibrary lib;
    hash_func_t hash_256_func;
    hash_func_t hash_512_func;
    encrypt_func_t encrypt_func;
    decrypt_func_t decrypt_func;
    gen_func_t gen_func;
};

#endif // CRYPTO_H
