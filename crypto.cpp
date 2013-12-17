#include "crypto.h"

#include <QDebug>

Crypto::Crypto(const QString &dirPath)
{
    lib.setFileName(dirPath);
    hash_256_func = (hash_func_t)lib.resolve("hash_256");
    hash_512_func = (hash_func_t)lib.resolve("hash_512");
    encrypt_func = (encrypt_func_t)lib.resolve("encrypt_aes256_ecb");
    decrypt_func = (decrypt_func_t)lib.resolve("decrypt_aes256_ecb");
    gen_func = (gen_func_t)lib.resolve("PRGB");
}

QVector<char>
Crypto::hash_256(const QString &msg)
{
    QVector<char> out(32);
    hash_256_func((unsigned char *)msg.data(),
                  msg.length(),
                  (unsigned char *)out.data());
    return out;
}

QVector<char>
Crypto::hash_512(const QString &msg)
{
    QVector<char> out(32);
    hash_512_func((unsigned char *)msg.data(),
                  msg.length(),
                  (unsigned char *)out.data());
    return out;
}

QVector<char>
Crypto::encrypt(const QString &msg, const QVector<char> &key)
{
    QVector<char> out(msg.size() - msg.size() % 16 + 16);
    encrypt_func((unsigned char *)msg.data(),
                 msg.size(),
                 (unsigned char *)key.data(),
                 (unsigned char *)out.data());
    return out;
}

QString
Crypto::decrypt(const QVector<char> &msg, const QVector<char> &key)
{
    unsigned long long length = msg.size();
    char *out = (char *)calloc(length, sizeof(*out));
    decrypt_func((unsigned char *)msg.data(),
                 msg.size(),
                 (unsigned char *)key.data(),
                 (unsigned char *)out,
                 &length);
    out = (char *)realloc(out, length);
    QString r(out);
    free(out);
    return r;
}

QVector<char>
Crypto::generate_next()
{
    QVector<char> key(32);
    gen_func((unsigned char *)key.data(), key.size());
    return key;
}
