#include <stdlib.h>
#include <stdio.h>

#include "aes.h"

byte_t s_table[16][16];
byte_t inv_s_table[16][16];

byte_t predefined_matrix[4][4] = {{0x02, 0x03, 0x01, 0x01},
                                  {0x01, 0x02, 0x03, 0x01},
                                  {0x01, 0x01, 0x02, 0x03},
                                  {0x03, 0x01, 0x01, 0x02}};
byte_t inv_predefined_matrix[4][4] = {{0x0e, 0x0b, 0x0d, 0x09},
                                      {0x09, 0x0e, 0x0b, 0x0d},
                                      {0x0d, 0x09, 0x0e, 0x0b},
                                      {0x0b, 0x0d, 0x09, 0x0e}};

byte_t galois_table_2[256];
byte_t galois_table_3[256];
byte_t galois_table_9[256];
byte_t galois_table_b[256];
byte_t galois_table_d[256];
byte_t galois_table_e[256];

byte_t key[BLOCK_SIZE];
byte_t iv[BLOCK_SIZE];
byte_t round_constants[ROUNDS] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};
byte_t keys[ROUNDS + 1][BLOCK_SIZE];

void init_table(const char *const filename, byte_t table[16][16], 
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

void init_key(const char *const filename, byte_t *array, const size_t size)
{
    FILE *f = fopen(filename, "rb");
    fread(array, sizeof(byte_t), size, f);
    fclose(f);
}

void init_array(const char *const filename, byte_t *array, const size_t size)
{
    FILE *f = fopen(filename, "r");

    for (size_t i = 0; i < size; ++i)
    {
        fscanf(f, "%hhx", &array[i]);
    }

    fclose(f);
}

static void sub_bytes(byte_t *block, byte_t table[16][16], const size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        block[i] = table[block[i] >> 4][block[i] & 15];
    }
}

static void copy_block(byte_t *dst, const byte_t *const src, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        dst[i] = src[i];
    }
}

static void transpose_block(byte_t *block, const size_t size)
{
    byte_t tmp[BLOCK_SIZE];
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

static void shift_rows_left(byte_t *block)
{
    transpose_block(block, BLOCK_SIZE);
    
    // second row
    byte_t tmp = block[4];
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

static void shift_rows_right(byte_t *block)
{
    transpose_block(block, BLOCK_SIZE);
    // second row
    byte_t tmp = block[7];
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

static byte_t galois_mul(const byte_t a, const byte_t b)
{
    if (a == 0x01)
        return b;
    else if (a == 0x02)
        return galois_table_2[b];
    else if (a == 0x03)
        return galois_table_3[b];

    return 0x00;
}

static byte_t inv_galois_mul(const byte_t a, const byte_t b)
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

static void mix_columns(byte_t *block, byte_t *result)
{
    transpose_block(block, BLOCK_SIZE);
    for (size_t i = 0, j = 0; j < 4; ++i, ++j)
    {
        for (size_t k = 0, l = 0; k < 4; ++k, l += 4)
        {
            byte_t byte_1 = galois_mul(predefined_matrix[k][0], block[0 + j]);
            byte_t byte_2 = galois_mul(predefined_matrix[k][1], block[4 + j]);
            byte_t byte_3 = galois_mul(predefined_matrix[k][2], block[8 + j]);
            byte_t byte_4 = galois_mul(predefined_matrix[k][3], block[12 + j]);
            
            result[l + i] = byte_1 ^ byte_2 ^ byte_3 ^ byte_4;
        }
    }
    transpose_block(result, BLOCK_SIZE);
}

static void inv_mix_columns(byte_t *block, byte_t *result)
{
    transpose_block(block, BLOCK_SIZE);
    for (size_t i = 0, j = 0; j < 4; ++i, ++j)
    {
        for (size_t k = 0, l = 0; k < 4; ++k, l += 4)
        {
            byte_t byte_1 = inv_galois_mul(inv_predefined_matrix[k][0], block[0 + j]);
            byte_t byte_2 = inv_galois_mul(inv_predefined_matrix[k][1], block[4 + j]);
            byte_t byte_3 = inv_galois_mul(inv_predefined_matrix[k][2], block[8 + j]);
            byte_t byte_4 = inv_galois_mul(inv_predefined_matrix[k][3], block[12 + j]);
            
            result[l + i] = byte_1 ^ byte_2 ^ byte_3 ^ byte_4;
        }
    }
    transpose_block(result, BLOCK_SIZE);
}

void _xor(byte_t *block, byte_t *key, byte_t *result, const size_t size)
{
    for (size_t i = 0 ; i < size; ++i)
    {
        result[i] = block[i] ^ key[i];
    }
}

static void rotate_key(byte_t *key, byte_t *result)
{
    result[0] = key[13];
    result[1] = key[14];
    result[2] = key[15];
    result[3] = key[12];
}

static void add_round_const(byte_t *key, const size_t round)
{
    key[0] = key[0] ^ round_constants[round];
}

static void generate_keys(const byte_t *const key)
{
    byte_t tmp[BLOCK_SIZE];
    copy_block(tmp, key, BLOCK_SIZE);
    copy_block(keys[0], tmp, BLOCK_SIZE);

    for (size_t i = 1; i <= ROUNDS; ++i)
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

void encrypt(const byte_t *const plain_block, byte_t *cipher_block, byte_t *key)
{
    byte_t tmp[BLOCK_SIZE];
    copy_block(tmp, plain_block, BLOCK_SIZE);
    // generate keys for 10 rounds
    generate_keys(key);
    // initial XOR
    _xor(tmp, keys[0], tmp, BLOCK_SIZE);
    
    for (size_t i = 1; i <= ROUNDS; ++i)
    {
        // substitute bytes
        sub_bytes(tmp, s_table, BLOCK_SIZE);
        // shift rows
        shift_rows_left(tmp);
        // mix column
        if (i != ROUNDS)
            mix_columns(tmp, cipher_block);
        else
            copy_block(cipher_block, tmp, BLOCK_SIZE);
        // add round key
        _xor(cipher_block, keys[i], tmp, BLOCK_SIZE);
    }

    copy_block(cipher_block, tmp, BLOCK_SIZE);
}

void decrypt(const byte_t *const plain_block, byte_t *cipher_block, byte_t *key)
{
    byte_t tmp[BLOCK_SIZE];
    copy_block(tmp, plain_block, BLOCK_SIZE);
    // generate keys for 10 rounds
    generate_keys(key);
    
    for (size_t i = 1; i <= ROUNDS; ++i)
    {
        // add round key
        _xor(tmp, keys[ROUNDS - i + 1], tmp, BLOCK_SIZE);
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
