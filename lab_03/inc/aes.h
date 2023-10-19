#ifndef _AES_H_
#define _AES_H_

#define BLOCK_SIZE 16

#define ROUNDS_128 10
#define ROUNDS_192 12
#define ROUNDS_256 14

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
extern byte_t key[32];
extern byte_t iv[BLOCK_SIZE];
extern byte_t round_constants[15];
extern byte_t keys[20][BLOCK_SIZE];

void init_table(const char *const filename, byte_t table[16][16], 
                const size_t rows, const size_t cols);
size_t init_key(const char *const filename, byte_t *array);
void init_array(const char *const filename, byte_t *array, const size_t size);
void encrypt(const byte_t *const plain_block, byte_t *cipher_block, const size_t rounds);
void decrypt(const byte_t *const plain_block, byte_t *cipher_block, const size_t rounds);
void _xor(byte_t *block, byte_t *key, byte_t *result, const size_t size);
void generate_keys_128(const byte_t *const key);
void generate_keys_192(const byte_t *const key);
void generate_keys_256(const byte_t *const key);

#endif // _AES_H_
