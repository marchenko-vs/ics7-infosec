#ifndef _RSA_H_
#define _RSA_H_

void rsa_generate_keys(const char first_prime[], const char second_prime[]);
void rsa_encrypt(FILE *f, const char message[], const char exponent[], const char modulus[]);

#endif // _RSA_H_
