#ifndef CRYPTO_H
#define CRYPTO_H

#include <QLibrary>

class Crypto
{
public:
    Crypto(const QString &dirPath);

    QVector<char> hash_256(const QString &msg);
    QVector<char> hash_512(const QString &msg);

    QVector<char> encrypt(const QString &msg, const QVector<char> &key);
    QString decrypt(const QVector<char> &msg, const QVector<char> &key);
    QVector<char> generate_next();

private:
    typedef void (*hash_func_t) (const unsigned char *, unsigned long long, unsigned char *);
    typedef int (*encrypt_func_t) (unsigned char *, unsigned long long, unsigned char *, unsigned char *);
    typedef int (*decrypt_func_t) (unsigned char *, unsigned long long, unsigned char *, unsigned char *, unsigned long long *);
    typedef int (*gen_func_t) (unsigned char *, unsigned long long);

    QLibrary lib;
    hash_func_t hash_256_func;
    hash_func_t hash_512_func;
    encrypt_func_t encrypt_func;
    decrypt_func_t decrypt_func;
    gen_func_t gen_func;
};

#endif // CRYPTO_H
