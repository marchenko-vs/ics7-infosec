#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "sha1.h"
#include "rsa.h"

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
        printf("  --rsa-keys                          generate RSA keys\n");
        printf("  --rsa exp mod INFILE OUTFILE        encrypt file with RSA\n");
        printf("  --sha INFILE OUTFILE                count hash of file\n");
    }
    else if (strcmp(argv[1], "--rsa-keys") == 0)
    {
        char p[1024], q[1024];
        FILE *f_p, *f_q;

        if ((f_p = fopen("cfg/p.txt", "r")) != NULL)
        {
            ssize_t len = fread(&p, sizeof(char), 1024 - 1, f_p);
            p[len - 1] = '\0';
            fclose(f_p);
        }

        if ((f_q = fopen("cfg/q.txt", "r")) != NULL)
        {
            ssize_t len = fread(&q, sizeof(char), 1024 - 1, f_q);
            q[len - 1] = '\0';
            fclose(f_q);
        }

        rsa_generate_keys(p, q);
    }
    else if (strcmp(argv[1], "--rsa") == 0)
    {
        if (argc != 6)
        {
            printf("Error: incorrect number of parameters.\n");
            printf("Use -h or --help for description.\n");
            return 1;
        }

        char exp[1024], mod[1024], in[1024];
        FILE *f_exp, *f_mod, *f_in, *f_out;

        if ((f_exp = fopen(argv[2], "r")) != NULL)
        {
            ssize_t len = fread(&exp, sizeof(char), 1024 - 1, f_exp);
            exp[len - 1] = '\0';
            fclose(f_exp);
        }

        if ((f_mod = fopen(argv[3], "r")) != NULL)
        {
            ssize_t len = fread(&mod, sizeof(char), 1024 - 1, f_mod);
            mod[len - 1] = '\0';
            fclose(f_mod);
        }

        if ((f_in = fopen(argv[4], "r")) != NULL)
        {
            ssize_t len = fread(&in, sizeof(char), 1024 - 1, f_in);
            in[len - 1] = '\0';
            fclose(f_in);
        }

        if ((f_out = fopen(argv[5], "w")) != NULL)
        {
            rsa_encrypt(f_out, in, exp, mod);
            fclose(f_out);
        }
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
