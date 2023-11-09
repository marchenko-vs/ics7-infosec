#include <stdio.h>
#include <stdint.h>

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

void print_uint8_t(const uint8_t *arr, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
        printf("%x\n", arr[i]);
}

void print_uint32_t(const uint32_t *arr, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
        printf("%x\n", arr[i]);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        return -1;

    uint8_t message[64];
    FILE *f = fopen(argv[1], "rb");
    fread(&message, sizeof(uint8_t), 64, f);
    fclose(f);

    return 0;
}
