#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "aes.h"

void fill_buffer(uint8_t *buffer, const size_t size);

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Error: program requires 2 parameters.\n");
        return -1;    
    }

    init_table("cfg/s-table.txt", s_table, BLOCK_SIZE, BLOCK_SIZE);
    init_table("cfg/inv-s-table.txt", inv_s_table, BLOCK_SIZE, BLOCK_SIZE);
    init_array("cfg/galois-table-2.txt", galois_table_2, 256);
    init_array("cfg/galois-table-3.txt", galois_table_3, 256);
    init_array("cfg/galois-table-9.txt", galois_table_9, 256);
    init_array("cfg/galois-table-b.txt", galois_table_b, 256);
    init_array("cfg/galois-table-d.txt", galois_table_d, 256);
    init_array("cfg/galois-table-e.txt", galois_table_e, 256);
    init_key("cfg/iv.txt", iv);

    size_t key_len = init_key("cfg/key.txt", key);
    size_t rounds = 0;

    if (key_len == 16)
    {
        generate_keys_128(key);
        rounds = ROUNDS_128;
    }
    else if (key_len == 24)
    {
        generate_keys_192(key);
        rounds = ROUNDS_192;
    }
    else if (key_len == 32)
    {
        generate_keys_256(key);
        rounds = ROUNDS_256;
    }
    else
    {
        printf("Error: incorrect length of the key.\n");
        return -2;
    }

    uint8_t plain_block[BLOCK_SIZE];
    uint8_t cipher_iv[BLOCK_SIZE];
    uint8_t cipher_block[BLOCK_SIZE];

    FILE *f_in = fopen(argv[1], "rb");
    size_t len = fread(plain_block, sizeof(uint8_t), BLOCK_SIZE, f_in);

    if (len == 0)
    {
        printf("Error: input file is empty.\n");
        return -2;
    }

    FILE *f_out = fopen(argv[2], "wb");

    if (len < 16)
        fill_buffer(plain_block, len);

    encrypt(iv, cipher_iv, rounds);
    _xor(plain_block, cipher_iv, cipher_block, BLOCK_SIZE);
    fwrite(cipher_block, sizeof(uint8_t), BLOCK_SIZE, f_out);
    len = fread(plain_block, sizeof(uint8_t), BLOCK_SIZE, f_in);

    while (len != 0)
    {
        if (len < 16)
            fill_buffer(plain_block, len);

        encrypt(cipher_iv, cipher_iv, rounds);
        _xor(plain_block, cipher_iv, cipher_block, BLOCK_SIZE);
        fwrite(cipher_block, sizeof(uint8_t), BLOCK_SIZE, f_out);

        len = fread(plain_block, sizeof(uint8_t), BLOCK_SIZE, f_in);
    }

    fclose(f_out);
    fclose(f_in);

    return 0;
}

void fill_buffer(uint8_t *buffer, const size_t size)
{
    for (size_t i = size; i < BLOCK_SIZE; ++i)
    {
        buffer[i] = 0;
    }
}
