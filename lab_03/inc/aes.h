#ifndef _AES_H_
#define _AES_H_

#define BLOCK_SIZE 16
#define ROUNDS 10

typedef unsigned char byte_t;

extern byte_t s_table[16][16];
extern byte_t inv_s_table[16][16];
extern byte_t predefined_matrix[4][4];
extern byte_t inv_predefined_matrix[4][4];
extern byte_t galois_table_2[256];
extern byte_t galois_table_3[256];
extern byte_t galois_table_9[256];
extern byte_t galois_table_b[256];
extern byte_t galois_table_d[256];
extern byte_t galois_table_e[256];
extern byte_t key[BLOCK_SIZE];
extern byte_t iv[BLOCK_SIZE];
extern byte_t round_constants[ROUNDS];
extern byte_t keys[ROUNDS + 1][BLOCK_SIZE];

void init_table(const char *const filename, byte_t table[16][16], 
                const size_t rows, const size_t cols);
void init_key(const char *const filename, byte_t *array, const size_t size);
void init_array(const char *const filename, byte_t *array, const size_t size);
void encrypt(const byte_t *const plain_block, byte_t *cipher_block, byte_t *key);
void decrypt(const byte_t *const plain_block, byte_t *cipher_block, byte_t *key);
void _xor(byte_t *block, byte_t *key, byte_t *result, const size_t size);

#endif // _AES_H_
