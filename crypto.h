#ifndef CRYPTO_H
#define CRYPTO_H

#include <QLibrary>
#define CRYPTO_PATH "AES256.dll"

class Crypto
{
public:
    static Crypto &getInstance();

    QByteArray hash_256(const QByteArray &msg);
    QByteArray hash_512(const QByteArray &msg);

    QByteArray encrypt(const QByteArray &msg, const QByteArray &key);
    QByteArray decrypt(const QByteArray &msg, const QByteArray &key);
    QByteArray generate_next();

    bool isReady() const;

private:
    Crypto();

    // Not implement, forbid usage.
    Crypto(const Crypto &);
    void operator=(const Crypto &);

    typedef void (*hash_func_t) (const unsigned char *, unsigned long long, unsigned char *);
    typedef int (*encrypt_func_t) (unsigned char *, unsigned long, unsigned char *, unsigned char *);
    typedef int (*decrypt_func_t) (unsigned char *, unsigned long, unsigned char *, unsigned char *, unsigned long *);
    typedef int (*gen_func_t) (unsigned char *, unsigned long);

    bool ready;

    QLibrary lib;
    hash_func_t hash_256_func;
    hash_func_t hash_512_func;
    encrypt_func_t encrypt_func;
    decrypt_func_t decrypt_func;
    gen_func_t gen_func;
};

#endif // CRYPTO_H
