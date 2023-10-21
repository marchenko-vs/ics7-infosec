#include <stdlib.h>
#include <stdio.h>

#include "aes.h"

uint8_t s_table[16][16];
uint8_t inv_s_table[16][16];

uint8_t predefined_matrix[4][4] = {{0x02, 0x03, 0x01, 0x01},
                                  {0x01, 0x02, 0x03, 0x01},
                                  {0x01, 0x01, 0x02, 0x03},
                                  {0x03, 0x01, 0x01, 0x02}};
uint8_t inv_predefined_matrix[4][4] = {{0x0e, 0x0b, 0x0d, 0x09},
                                      {0x09, 0x0e, 0x0b, 0x0d},
                                      {0x0d, 0x09, 0x0e, 0x0b},
                                      {0x0b, 0x0d, 0x09, 0x0e}};

uint8_t galois_table_2[256];
uint8_t galois_table_3[256];
uint8_t galois_table_9[256];
uint8_t galois_table_b[256];
uint8_t galois_table_d[256];
uint8_t galois_table_e[256];

uint8_t key[32];
uint8_t iv[BLOCK_SIZE];

uint8_t round_constants[15] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 
                              0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a};
uint8_t keys[20][BLOCK_SIZE];

void init_table(const char *const filename, uint8_t table[16][16], 
                const size_t rows, const size_t cols)
{
    FILE *f = fopen(filename, "r");

    for (size_t i = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j)
        {
            fscanf(f, "%hhx", &table[i][j]);
        }
    }

    fclose(f);
}

size_t init_key(const char *const filename, uint8_t *array)
{
    FILE *f = fopen(filename, "rb");
    size_t len = fread(array, sizeof(uint8_t), 32, f);
    fclose(f);

    return len;
}

void init_array(const char *const filename, uint8_t *array, const size_t size)
{
    FILE *f = fopen(filename, "r");

    for (size_t i = 0; i < size; ++i)
    {
        fscanf(f, "%hhx", &array[i]);
    }

    fclose(f);
}

static void sub_bytes(uint8_t *block, uint8_t table[16][16], const size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        block[i] = table[block[i] >> 4][block[i] & 15];
    }
}

void copy_block(uint8_t *dst, const uint8_t *const src, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        dst[i] = src[i];
    }
}

static void transpose_block(uint8_t *block, const size_t size)
{
    uint8_t tmp[BLOCK_SIZE];
    copy_block(tmp, block, size);

    for (size_t i = 0, j = 0; i < 4; ++i, j += 4)
    {
        tmp[i + 0] = block[j + 0];
        tmp[i + 4] = block[j + 1];
        tmp[i + 8] = block[j + 2];
        tmp[i + 12] = block[j + 3];
    }

    copy_block(block, tmp, size);
}

static void shift_rows_left(uint8_t *block)
{
    transpose_block(block, BLOCK_SIZE);
    
    // second row
    uint8_t tmp = block[4];
    block[4] = block[5];
    block[5] = block[6];
    block[6] = block[7];
    block[7] = tmp;
    // third row
    tmp = block[8];
    block[8] = block[10];
    block[10] = tmp;
    tmp = block[9];
    block[9] = block[11];
    block[11] = tmp;
    // fourth row
    tmp = block[12];
    block[12] = block[15];
    block[15] = block[14];
    block[14] = block[13];
    block[13] = tmp;
    
    transpose_block(block, BLOCK_SIZE);
}

static void shift_rows_right(uint8_t *block)
{
    transpose_block(block, BLOCK_SIZE);
    // second row
    uint8_t tmp = block[7];
    block[7] = block[6];
    block[6] = block[5];
    block[5] = block[4];
    block[4] = tmp;
    // third row
    tmp = block[10];
    block[10] = block[8];
    block[8] = tmp;
    tmp = block[11];
    block[11] = block[9];
    block[9] = tmp;
    // fourth row
    tmp = block[12];
    block[12] = block[13];
    block[13] = block[14];
    block[14] = block[15];
    block[15] = tmp;

    transpose_block(block, BLOCK_SIZE);
}

static uint8_t galois_mul(const uint8_t a, const uint8_t b)
{
    if (a == 0x01)
        return b;
    else if (a == 0x02)
        return galois_table_2[b];
    else if (a == 0x03)
        return galois_table_3[b];

    return 0x00;
}

static uint8_t inv_galois_mul(const uint8_t a, const uint8_t b)
{
    if (a == 0x09)
        return galois_table_9[b];
    else if (a == 0x0b)
        return galois_table_b[b];
    else if (a == 0x0d)
        return galois_table_d[b];
    else if (a == 0x0e)
        return galois_table_e[b];

    return 0x00;
}

static void mix_columns(uint8_t *block, uint8_t *result)
{
    transpose_block(block, BLOCK_SIZE);
    for (size_t i = 0, j = 0; j < 4; ++i, ++j)
    {
        for (size_t k = 0, l = 0; k < 4; ++k, l += 4)
        {
            uint8_t byte_1 = galois_mul(predefined_matrix[k][0], block[0 + j]);
            uint8_t byte_2 = galois_mul(predefined_matrix[k][1], block[4 + j]);
            uint8_t byte_3 = galois_mul(predefined_matrix[k][2], block[8 + j]);
            uint8_t byte_4 = galois_mul(predefined_matrix[k][3], block[12 + j]);
            
            result[l + i] = byte_1 ^ byte_2 ^ byte_3 ^ byte_4;
        }
    }
    transpose_block(result, BLOCK_SIZE);
}

static void inv_mix_columns(uint8_t *block, uint8_t *result)
{
    transpose_block(block, BLOCK_SIZE);
    for (size_t i = 0, j = 0; j < 4; ++i, ++j)
    {
        for (size_t k = 0, l = 0; k < 4; ++k, l += 4)
        {
            uint8_t byte_1 = inv_galois_mul(inv_predefined_matrix[k][0], block[0 + j]);
            uint8_t byte_2 = inv_galois_mul(inv_predefined_matrix[k][1], block[4 + j]);
            uint8_t byte_3 = inv_galois_mul(inv_predefined_matrix[k][2], block[8 + j]);
            uint8_t byte_4 = inv_galois_mul(inv_predefined_matrix[k][3], block[12 + j]);
            
            result[l + i] = byte_1 ^ byte_2 ^ byte_3 ^ byte_4;
        }
    }
    transpose_block(result, BLOCK_SIZE);
}

void _xor(uint8_t *block, uint8_t *key, uint8_t *result, const size_t size)
{
    for (size_t i = 0 ; i < size; ++i)
    {
        result[i] = block[i] ^ key[i];
    }
}

static void rotate_key(uint8_t *key, uint8_t *result)
{
    result[0] = key[13];
    result[1] = key[14];
    result[2] = key[15];
    result[3] = key[12];
}

static void add_round_const(uint8_t *key, const size_t round)
{
    key[0] = key[0] ^ round_constants[round];
}

void generate_keys_128(const uint8_t *const key)
{
    uint8_t tmp[BLOCK_SIZE];
    copy_block(tmp, key, BLOCK_SIZE);
    copy_block(keys[0], tmp, BLOCK_SIZE);

    for (size_t i = 1; i <= ROUNDS_128; ++i)
    {
        rotate_key(tmp, keys[i]);
        sub_bytes(keys[i], s_table, 4);
        add_round_const(keys[i], i - 1);
        _xor(keys[i], tmp, keys[i], 4);
        _xor(keys[i], tmp + 4, keys[i] + 4, 4);
        _xor(keys[i] + 4, tmp + 8, keys[i] + 8, 4);
        _xor(keys[i] + 8, tmp + 12, keys[i] + 12, 4);
        copy_block(tmp, keys[i], BLOCK_SIZE);
    }
}

void generate_keys_192(const uint8_t *const key)
{
    uint8_t tmp[16];
    uint8_t memory[8];
    copy_block(tmp, key, 16);
    copy_block(memory, key + 16, 8);

    copy_block(keys[0], tmp, 16);

    for (size_t i = 1; i <= ROUNDS_192; ++i)
    {
        rotate_key(tmp, keys[i]);
        sub_bytes(keys[i], s_table, 4);
        add_round_const(keys[i], i - 1);
        _xor(keys[i], tmp, keys[i], 4);
        _xor(keys[i], tmp + 4, keys[i] + 4, 4);
        _xor(keys[i] + 4, tmp + 8, keys[i] + 8, 4);
        _xor(keys[i] + 8, tmp + 12, keys[i] + 12, 4);
        
        copy_block(tmp, memory, 8);
        copy_block(tmp + 8, keys[i], 16);
        copy_block(memory, keys[i] + 16, 8);
    }
}

void generate_keys_256(const uint8_t *const key)
{
    uint8_t tmp[16];
    uint8_t memory[16];
    copy_block(tmp, key, 16);
    copy_block(memory, key + 16, 16);

    copy_block(keys[0], tmp, 16);

    for (size_t i = 1; i <= ROUNDS_256; ++i)
    {
        rotate_key(tmp, keys[i]);
        sub_bytes(keys[i], s_table, 4);
        add_round_const(keys[i], i - 1);
        _xor(keys[i], tmp, keys[i], 4);
        _xor(keys[i], tmp + 4, keys[i] + 4, 4);
        _xor(keys[i] + 4, tmp + 8, keys[i] + 8, 4);
        _xor(keys[i] + 8, tmp + 12, keys[i] + 12, 4);
        
        copy_block(tmp, memory, 16);
        copy_block(memory, keys[i], 16);
    }
}

void encrypt(const uint8_t *const plain_block, uint8_t *cipher_block, const size_t rounds)
{
    uint8_t tmp[BLOCK_SIZE];
    copy_block(tmp, plain_block, BLOCK_SIZE);
    // initial XOR
    _xor(tmp, keys[0], tmp, BLOCK_SIZE);
    
    for (size_t i = 1; i <= rounds; ++i)
    {
        // substitute bytes
        sub_bytes(tmp, s_table, BLOCK_SIZE);
        // shift rows
        shift_rows_left(tmp);
        // mix column
        if (i != rounds)
            mix_columns(tmp, cipher_block);
        else
            copy_block(cipher_block, tmp, BLOCK_SIZE);
        // add round key
        _xor(cipher_block, keys[i], tmp, BLOCK_SIZE);
    }

    copy_block(cipher_block, tmp, BLOCK_SIZE);
}

void decrypt(const uint8_t *const plain_block, uint8_t *cipher_block, const size_t rounds)
{
    uint8_t tmp[BLOCK_SIZE];
    copy_block(tmp, plain_block, BLOCK_SIZE);
    
    for (size_t i = 1; i <= rounds; ++i)
    {
        // add round key
        _xor(tmp, keys[rounds - i + 1], tmp, BLOCK_SIZE);
        // mix column
        if (i != 1)
            inv_mix_columns(tmp, cipher_block);
        else
            copy_block(cipher_block, tmp, BLOCK_SIZE);
        // shift rows
        shift_rows_right(cipher_block);
        // substitute bytes
        sub_bytes(cipher_block, inv_s_table, BLOCK_SIZE);
        copy_block(tmp, cipher_block, BLOCK_SIZE);
    }

    // final XOR
    _xor(tmp, keys[0], cipher_block, BLOCK_SIZE);
}
