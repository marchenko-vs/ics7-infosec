#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "rsa.h"

bool is_prime(const uint32_t n)
{
    if (n == 2 || n == 3)
        return true;

    if (n <= 1 || n % 2 == 0 || n % 3 == 0)
        return false;

    for (size_t i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
            return false;
    }

    return true;
}

static uint32_t gcd(uint32_t n1, uint32_t n2)
{
    uint32_t gcd = 0;

    for(size_t i = 1; i <= n1 && i <= n2; ++i)
    {
        if (n1 % i == 0 && n2 % i == 0)
            gcd = i;
    }

    return gcd;
}

uint64_t rsa_encrypt(uint64_t a, uint64_t b, uint64_t m)
{
    uint64_t res = 1;

    for (uint64_t i = 0; i < b; ++i)
    {
        res *= a;
        res %= m;
    }

    return res;
}

int rsa_generate_keys(const uint32_t p, const uint32_t q, uint32_t *e, uint32_t *d, uint32_t *n)
{
    if (!is_prime(p))
        return 1;
    if (!is_prime(q))
        return 2;

    uint32_t phi = (p - 1) * (q - 1);
    uint32_t public_key = 2;

    while (public_key < phi)
    {
        if (gcd(public_key, phi) == 1)
            break;
        ++public_key;
    }

    uint32_t private_key = 0;
    uint32_t mod = 0;
    
    while (true)
    {
        mod = ((private_key * public_key) % phi);

        if (mod == 1 && private_key != public_key)
            break;

        ++private_key;
    }

    *e = public_key;
    *d = private_key;
    *n = p * q;

    return 0;
}
