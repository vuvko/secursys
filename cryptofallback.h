#ifndef CryptoFallback_H
#define CryptoFallback_H

void hash_func_256_fallback(const unsigned char *, unsigned long long, unsigned char *out)
{
    memset(out, 1, 32);
}

void hash_func_512_fallback(const unsigned char *, unsigned long long, unsigned char *out)
{
    memset(out, 1, 64);
}

int encrypt_func_fallback(unsigned char *f, unsigned long c, unsigned char *, unsigned char *out)
{
    memcpy(out, f, c);
    if (c % 16)
        memset(out + c, 0, 16 - c % 16);
    return c;
}

int decrypt_func_fallback(unsigned char *f, unsigned long c, unsigned char *, unsigned char *out, unsigned long *)
{
    memcpy(out, f, c);
    return c;
}

int gen_func_fallback(unsigned char *out, unsigned long c)
{
    memset(out, 2, c);
    return c;
}

#endif // CryptoFallback_H
