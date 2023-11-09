#ifndef _SHA1_H_
#define _SHA1_H_

#include <stdint.h>
#include <stdio.h>

// hash values
extern uint32_t h_0, h_1, h_2, h_3, h_4;

void fprint_hash(FILE *f);
void padding(uint8_t block[64], const uint64_t size_in_bits);
void sha_1(uint8_t *message);
uint64_t file_size(const char* const filename);

#endif // _SHA1_H_
