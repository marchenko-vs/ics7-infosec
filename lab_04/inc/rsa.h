#ifndef _RSA_H_
#define _RSA_H_

int rsa_generate_keys(const uint32_t p, const uint32_t q, 
    uint32_t *e, uint32_t *d, uint32_t *n);
uint64_t rsa_encrypt(uint64_t a, uint64_t b, uint64_t m);

#endif // _RSA_H_
