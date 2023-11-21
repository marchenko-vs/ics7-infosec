#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <gmp.h>

#include "rsa.h"

void rsa_generate_keys(const char first_prime[], const char second_prime[])
{
    FILE *f_e = fopen("cfg/e.txt", "w");
    FILE *f_d = fopen("cfg/d.txt", "w");
    FILE *f_n = fopen("cfg/n.txt", "w");

    mpz_t p, q, e, d, n, phi;
    mpz_t p_phi, q_phi;
    mpz_t one, gcd, mul_val, mod_val;

    mpz_init_set_str(p, first_prime, 10);
    mpz_init_set_str(q, second_prime, 10);
    mpz_init_set_str(one, "1", 10);
    mpz_init_set_str(d, "2", 10);
    mpz_init(n);
    mpz_init(phi);
    mpz_init(p_phi);
    mpz_init(q_phi);
    mpz_init(gcd);
    mpz_init(mul_val);
    mpz_init(mod_val);

    mpz_mul(n, p, q);

    mpz_sub(p_phi, p, one);
    mpz_sub(q_phi, q, one);
    mpz_mul(phi, p_phi, q_phi);
    
    mpz_init_set_str(e, "65537", 10);
    
    mpz_gcdext(gcd, mod_val, mul_val, e, phi);
    mpz_mod(d, mod_val, phi);

    gmp_fprintf(f_e, "%Zd\n", e);
    gmp_fprintf(f_d, "%Zd\n", d);
    gmp_fprintf(f_n, "%Zd\n", n);

    mpz_clear(phi);
    mpz_clear(n);
    mpz_clear(d);
    mpz_clear(e);
    mpz_clear(p);
    mpz_clear(q);
    mpz_clear(p_phi);
    mpz_clear(q_phi);
    mpz_clear(one);
    mpz_clear(gcd);
    mpz_clear(mul_val);
    mpz_clear(mod_val);

    fclose(f_e);
    fclose(f_d);
    fclose(f_n);
}

void rsa_encrypt(FILE *f, const char message[], const char exponent[], const char modulus[])
{
    mpz_t m, e, n;
    mpz_t res;

    mpz_init_set_str(m, message, 16);
    mpz_init_set_str(e, exponent, 10);
    mpz_init_set_str(n, modulus, 10);
    mpz_init(res);

    mpz_powm(res, m, e, n);

    gmp_fprintf(f, "%Zx\n", res);

    mpz_clear(m);
    mpz_clear(e);
    mpz_clear(n);
    mpz_clear(res);
}
