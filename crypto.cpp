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

QString
Crypto::hash_256(const QString &msg)
{
    char *out = (char *)calloc(32, sizeof(*out));
    hash_256_func((unsigned char *)msg.toStdString().c_str(),
                  msg.length(),
                  (unsigned char *)out);
    return QString(out);
}

QString
Crypto::hash_512(const QString &msg)
{
    char *out = (char *)calloc(64, sizeof(*out));
    hash_512_func((unsigned char *)msg.toStdString().c_str(),
                  msg.length(),
                  (unsigned char *)out);
    return QString(out);
}

QString
Crypto::encrypt(const QString &msg, const QList<char> &key) {}

QString
Crypto::decrypt(const QString &msg, const QList<char> &key) {}

QList<char>
Crypto::generate_next()
{
    int length = 32;
    char *out = (char *)calloc(length, sizeof(*out));
    gen_func((unsigned char *)out, length);
    return QList<char>();
}
