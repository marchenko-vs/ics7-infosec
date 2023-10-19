#include <stdio.h>
#include <string.h>

#include "aes.h"

void fill_buffer(byte_t *buffer, const size_t size);

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Error: program requires 3 parameters.\n");
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
    init_key("cfg/key.txt", key, BLOCK_SIZE);
    init_key("cfg/iv.txt", iv, BLOCK_SIZE);

    byte_t plain_block[BLOCK_SIZE];
    byte_t cipher_iv[BLOCK_SIZE];
    byte_t cipher_block[BLOCK_SIZE];
    //size_t progress = 0;

    if (strcmp(argv[1], "-e") == 0)
    {
        FILE *f_in = fopen(argv[2], "rb");
        size_t len = fread(plain_block, sizeof(byte_t), BLOCK_SIZE, f_in);

        if (len == 0)
        {
            printf("Error: input file is empty.\n");
            return -2;
        }

        FILE *f_out = fopen(argv[3], "wb");

        if (len < 16)
            fill_buffer(plain_block, len);

        encrypt(iv, cipher_iv, key);
        _xor(plain_block, cipher_iv, cipher_block, BLOCK_SIZE);
        fwrite(cipher_block, sizeof(byte_t), BLOCK_SIZE, f_out);
        len = fread(plain_block, sizeof(byte_t), BLOCK_SIZE, f_in);

        while (len != 0)
        {
            if (len < 16)
                fill_buffer(plain_block, len);

            encrypt(cipher_iv, cipher_iv, key);
            _xor(plain_block, cipher_iv, cipher_block, BLOCK_SIZE);
            fwrite(cipher_block, sizeof(byte_t), BLOCK_SIZE, f_out);

            //progress++;
            //printf("Iteration: %zu\n", progress);

            len = fread(plain_block, sizeof(byte_t), BLOCK_SIZE, f_in);
        }

        fclose(f_out);
        fclose(f_in);
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        FILE *f_in = fopen(argv[2], "rb");
        size_t len = fread(cipher_block, sizeof(byte_t), BLOCK_SIZE, f_in);

        if (len == 0)
        {
            printf("Error: input file is empty.\n");
            return -2;
        }

        if (len < 16)
            fill_buffer(cipher_block, len);

        FILE *f_out = fopen(argv[3], "wb");

        encrypt(iv, cipher_iv, key);
        _xor(cipher_block, cipher_iv, plain_block, BLOCK_SIZE);
        fwrite(plain_block, sizeof(byte_t), BLOCK_SIZE, f_out);
        len = fread(cipher_block, sizeof(byte_t), BLOCK_SIZE, f_in);

        while (len != 0)
        {
            if (len < 16)
                fill_buffer(cipher_block, len);

            encrypt(cipher_iv, cipher_iv, key);
            _xor(cipher_block, cipher_iv, plain_block, BLOCK_SIZE);
            fwrite(plain_block, sizeof(byte_t), BLOCK_SIZE, f_out);

            //progress++;
            //printf("Iteration: %zu\n", progress);

            len = fread(cipher_block, sizeof(byte_t), BLOCK_SIZE, f_in);
        }
        
        fclose(f_out);
        fclose(f_in);
    }
    else
    {
        printf("Error: incorrect option %s.\n", argv[1]);
        return -1;  
    }

    return 0;
}

void fill_buffer(byte_t *buffer, const size_t size)
{
    for (size_t i = size; i < BLOCK_SIZE; ++i)
    {
        buffer[i] = 0;
    }
}
