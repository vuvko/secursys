#include "crypto.h"

Crypto &Crypto::getInstance()
{
    static Crypto instance;
    return instance;
}

Crypto::Crypto()
{
    lib.setFileName(CRYPTO_PATH);
    hash_256_func = (hash_func_t)lib.resolve("hash_256");
    hash_512_func = (hash_func_t)lib.resolve("hash_512");
    encrypt_func = (encrypt_func_t)lib.resolve("encrypt_aes256_ecb");
    decrypt_func = (decrypt_func_t)lib.resolve("decrypt_aes256_ecb");
    gen_func = (gen_func_t)lib.resolve("PRGB");

    ready = hash_256_func != 0 &&
        hash_512_func != 0 &&
        encrypt_func != 0 &&
        decrypt_func != 0 &&
        gen_func != 0;
}

bool Crypto::isReady() const
{
    return ready;
}

QByteArray
Crypto::hash_256(const QByteArray &msg)
{
    QByteArray out(32, 0);
    hash_256_func((unsigned char *)msg.data(),
                  msg.length(),
                  (unsigned char *)out.data());
    return out;
}

QByteArray
Crypto::hash_512(const QByteArray &msg)
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
    QByteArray out(((msg.size() + 15) / 16) * 16, 0);
    encrypt_func((unsigned char *)msg.data(),
                       msg.size(),
                       (unsigned char *)key.data(),
                       (unsigned char *)out.data());
    return out;
}

QByteArray
Crypto::decrypt(const QByteArray &msg, const QByteArray &key)
{
    unsigned long length = msg.size();
    QByteArray out(msg.size(), 0);
    decrypt_func((unsigned char *)msg.data(),
                 msg.size(),
                 (unsigned char *)key.data(),
                 (unsigned char *)out.data(),
                 &length);
    out.remove(length, out.size() - length);
    return out;
}

QByteArray
Crypto::generate_next()
{
    QByteArray key(32, 0);
    gen_func((unsigned char *)key.data(), key.size());
    return key;
}
