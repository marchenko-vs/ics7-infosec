#ifndef _SHA1_H_
#define _SHA1_H_

#include <stdint.h>
#include <stdio.h>

// hash values
extern uint32_t h_0, h_1, h_2, h_3, h_4;

void print_hash(void);
void sha_1(uint8_t *message);

uint8_t file_size(const char* const filename);

#endif // _SHA1_H_
