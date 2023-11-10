#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sha1.h"
#include "rsa.h"

void print_bin_8(const uint8_t num)
{
    uint32_t mask = 0b10000000;
    for (size_t i = 0; i < 8; ++i)
    {
        if ((num & mask) == mask)
            printf("1");
        else
            printf("0");
        mask >>= 1;
    }
    printf("\n");
}

void print_bin_32(const uint32_t num)
{
    uint32_t mask = 0b10000000000000000000000000000000;
    for (size_t i = 0; i < 32; ++i)
    {
        if ((num & mask) == mask)
            printf("1");
        else
            printf("0");
        mask >>= 1;
    }
    printf("\n");
}

void print_uint32_t(const uint32_t *arr, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
        printf("%x\n", arr[i]);
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("Error: no options were given.\n");
        printf("Use -h or --help for description.\n");
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: ./app.exe [OPTION]... [ARGUMENT]\n\n");
        printf("  --rsa-keys p q                      generate RSA keys\n");
        printf("  --rsa-encrypt e n INFILE OUTFILE    encrypt file with RSA\n");
        printf("  --rsa-decrypt d n INFILE OUTFILE    decrypt file with RSA\n");
        printf("  --sha INFILE OUTFILE                count hash of file\n");
    }
    else if (strcmp(argv[1], "--rsa-keys") == 0)
    {
        if (argc != 4)
        {
            printf("Error: incorrect number of parameters.\n");
            printf("Use -h or --help for description.\n");
            return 1;
        }

        uint32_t e, d, n;
        int p = atoi(argv[2]);
        int q = atoi(argv[3]);

        int err = rsa_generate_keys(p, q, &e, &d, &n);

        if (err)
        {
            printf("Error: both numbers must be prime integers.\n");
            return 3;
        }

        printf("RSA generated following keys.\n");
        printf("Public key: (%d, %d).\n", e, n);
        printf("Private key: (%d, %d).\n", d, n);
    }
    else if (strcmp(argv[1], "--rsa-encrypt") == 0)
    {
        if (argc != 6)
        {
            printf("Error: incorrect number of parameters.\n");
            printf("Use -h or --help for description.\n");
            return 1;
        }

        int key = atoi(argv[2]);
        int n = atoi(argv[3]);

        if (key == 0 || n == 0)
        {
            printf("Error: key is incorrect.\n");
            return 3;
        }

        FILE *f_in = fopen(argv[4], "r");
        if (!f_in)
        {
            printf("Error: file cannot be found.\n");
            return 2;
        }

        uint16_t byte;
        int len;
        char tmp;

        FILE *f_out = fopen(argv[5], "w");

        while ((len = fscanf(f_in, "%c", &tmp)) == 1)
        {
            byte = tmp;
            uint16_t encrypted_byte = rsa_encrypt(byte, key, n);
            fprintf(f_out, "%d ", encrypted_byte);
        }

        fclose(f_out);
        fclose(f_in);
    }
    else if (strcmp(argv[1], "--rsa-decrypt") == 0)
    {
        if (argc != 6)
        {
            printf("Error: incorrect number of parameters.\n");
            printf("Use -h or --help for description.\n");
            return 1;
        }

        int key = atoi(argv[2]);
        int n = atoi(argv[3]);

        if (key == 0 || n == 0)
        {
            printf("Error: key is incorrect.\n");
            return 3;
        }

        FILE *f_in = fopen(argv[4], "r");
        if (!f_in)
        {
            printf("Error: file cannot be found.\n");
            return 2;
        }

        uint16_t byte;
        int len;
        char tmp;

        FILE *f_out = fopen(argv[5], "w");

        while ((len = fscanf(f_in, "%hd", &byte)) == 1)
        {
            uint16_t encrypted_byte = rsa_encrypt(byte, key, n);
            tmp = encrypted_byte;
            fprintf(f_out, "%c", tmp);
        }

        fclose(f_out);
        fclose(f_in);
    }
    else if(strcmp(argv[1], "--sha") == 0)
    {
        if (argc != 4)
        {
            printf("Error: incorrect number of parameters.\n");
            printf("Use -h or --help for description.\n");
            return 1;
        }

        uint64_t filesize = file_size(argv[2]);
        FILE *f_in = fopen(argv[2], "rb");
        if (!f_in)
        {
            printf("Error: file cannot be found.\n");
            return 2;
        }

        uint8_t bytes[64];
        size_t len;
        bool extra_block = false;

        FILE *f_out = fopen(argv[3], "wb");

        while ((len = fread(&bytes, sizeof(uint8_t), 64, f_in)) != 0)
        {
            if (len < 64 && len >= 56)
                extra_block = true;

            if (len < 64)
                padding(bytes, filesize, extra_block);

            sha_1(bytes);
        }

        if (extra_block || filesize % 512 == 0)
        {
            for (size_t i = 0; i < 64; ++i)
                bytes[i] = 0;
            padding(bytes, filesize, false);
            sha_1(bytes);
        }

        fprint_hash(f_out);
        fclose(f_out);
        fclose(f_in);
    }
    else if(strcmp(argv[1], "--sha-cmp") == 0)
    {
        if (argc != 4)
        {
            printf("Error: incorrect number of parameters.\n");
            printf("Use -h or --help for description.\n");
            return 1;
        }

        uint64_t filesize = file_size(argv[2]);
        FILE *f_in = fopen(argv[2], "rb");
        if (!f_in)
        {
            printf("Error: file cannot be found.\n");
            return 2;
        }

        uint8_t bytes[64];
        size_t len;
        bool extra_block = false;

        FILE *f_out = fopen(argv[3], "wb");

        while ((len = fread(&bytes, sizeof(uint8_t), 64, f_in)) != 0)
        {
            if (len < 64 && len >= 56)
                extra_block = true;

            if (len < 64)
                padding(bytes, filesize, extra_block);

            sha_1(bytes);
        }

        if (extra_block || filesize % 512 == 0)
        {
            for (size_t i = 0; i < 64; ++i)
                bytes[i] = 0;
            padding(bytes, filesize, false);
            sha_1(bytes);
        }

        fprint_hash(f_out);
        fclose(f_out);
        fclose(f_in);
    }
    else
    {
        printf("Error: incorrect option.\n");
        printf("Use -h or --help for description.\n");
        return 1;
    }

    return 0;
}
