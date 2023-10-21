#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

#define BLOCK_SIZE 16

#define ROUNDS_128 10
#define ROUNDS_192 12
#define ROUNDS_256 14

extern uint8_t s_table[16][16];
extern uint8_t inv_s_table[16][16];
extern uint8_t predefined_matrix[4][4];
extern uint8_t inv_predefined_matrix[4][4];
extern uint8_t galois_table_2[256];
extern uint8_t galois_table_3[256];
extern uint8_t galois_table_9[256];
extern uint8_t galois_table_b[256];
extern uint8_t galois_table_d[256];
extern uint8_t galois_table_e[256];
extern uint8_t key[32];
extern uint8_t iv[BLOCK_SIZE];
extern uint8_t round_constants[15];
extern uint8_t keys[20][BLOCK_SIZE];

void init_table(const char *const filename, uint8_t table[16][16], 
                const size_t rows, const size_t cols);
size_t init_key(const char *const filename, uint8_t *array);
void init_array(const char *const filename, uint8_t *array, const size_t size);
void encrypt(const uint8_t *const plain_block, uint8_t *cipher_block, const size_t rounds);
void decrypt(const uint8_t *const plain_block, uint8_t *cipher_block, const size_t rounds);
void _xor(uint8_t *block, uint8_t *key, uint8_t *result, const size_t size);
void generate_keys_128(const uint8_t *const key);
void generate_keys_192(const uint8_t *const key);
void generate_keys_256(const uint8_t *const key);
void copy_block(uint8_t *dst, const uint8_t *const src, const size_t size);

#endif // _AES_H_
