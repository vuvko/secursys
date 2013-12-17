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

QByteArray
Crypto::hash_256(const QString &msg)
{
    QByteArray out(32, 0);
    hash_256_func((unsigned char *)msg.data(),
                  msg.length(),
                  (unsigned char *)out.data());
    return out;
}

QByteArray
Crypto::hash_512(const QString &msg)
{
    QByteArray out(64, 0);
    hash_512_func((unsigned char *)msg.data(),
                  msg.length(),
                  (unsigned char *)out.data());
    return out;
}

QByteArray
Crypto::encrypt(const QByteArray &msg, const QByteArray &key)
{
    QByteArray out(msg.size() - msg.size() % 16 + 16, 0);
    encrypt_func((unsigned char *)msg.data(),
                 msg.size(),
                 (unsigned char *)key.data(),
                 (unsigned char *)out.data());
    return out;
}

QByteArray
Crypto::decrypt(const QByteArray &msg, const QByteArray &key)
{
    unsigned long long length = msg.size();
    QByteArray out(msg.size(), 0);
    decrypt_func((unsigned char *)msg.data(),
                 msg.size(),
                 (unsigned char *)key.data(),
                 (unsigned char *)out.data(),
                 &length);
    return out;
}

QByteArray
Crypto::generate_next()
{
    QByteArray key(32, 0);
    gen_func((unsigned char *)key.data(), key.size());
    return key;
}
