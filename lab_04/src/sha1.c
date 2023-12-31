#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>

#include "sha1.h"

uint32_t k_0 = 0x5a827999;
uint32_t k_1 = 0x6ed9eba1; 
uint32_t k_2 = 0x8f1bbcdc;
uint32_t k_3 = 0xca62c1d6;

uint32_t h_0 = 0x67452301;
uint32_t h_1 = 0xefcdab89;
uint32_t h_2 = 0x98badcfe;
uint32_t h_3 = 0x10325476; 
uint32_t h_4 = 0xc3d2e1f0;

void fprint_hash(FILE *f)
{
    fprintf(f, "%"PRIx32, h_0);
    fprintf(f, "%"PRIx32, h_1);
    fprintf(f, "%"PRIx32, h_2);
    fprintf(f, "%"PRIx32, h_3);
    fprintf(f, "%"PRIx32"\n", h_4);
}

void split_block(const uint8_t *const src, uint32_t *dst)
{
    for (size_t i = 0; i < 16; ++i)
    {
        dst[i] = 0;
        dst[i] += ((0x00000000 | src[4 * i]) << 24);
        dst[i] += ((0x00000000 | src[4 * i + 1]) << 16);
        dst[i] += ((0x00000000 | src[4 * i + 2]) << 8);
        dst[i] += ((0x00000000 | src[4 * i + 3]));
    }
}

uint32_t f_0(const uint32_t b, const uint32_t c, const uint32_t d)
{
    return (b & c) | ((~b) & d);
}

uint32_t f_1(const uint32_t b, const uint32_t c, const uint32_t d)
{
    return b ^ c ^ d;
}

uint32_t f_2(const uint32_t b, const uint32_t c, const uint32_t d)
{
    return (b & c) | (b & d) | (c & d);
}

uint32_t f_3(const uint32_t b, const uint32_t c, const uint32_t d)
{
    return b ^ c ^ d;
}

uint32_t shift_n(const uint32_t num, const size_t n)
{
    if (n > 32)
        return 0;

    return ((num << n) | (num >> (32 - n)));
}

void sha_1(uint8_t *message)
{
    uint32_t words[80];

    uint32_t a = h_0;
    uint32_t b = h_1;
    uint32_t c = h_2;
    uint32_t d = h_3;
    uint32_t e = h_4;

    split_block(message, words);
    for (size_t i = 16; i < 80; ++i)
    {
        words[i] = (words[i - 3] ^ words[i - 8] ^ words[i - 14] ^ words[i - 16]);
        words[i] = shift_n(words[i], 1);
    }

    for (size_t i = 0; i < 20; ++i)
    {
        uint32_t tmp = shift_n(a, 5) + f_0(b, c, d) + e + words[i] + k_0;
        e = d;
        d = c;
        c = shift_n(b, 30);
        b = a;
        a = tmp;
    }

    for (size_t i = 20; i < 40; ++i)
    {
        uint32_t tmp = shift_n(a, 5) + f_1(b, c, d) + e + words[i] + k_1;
        e = d;
        d = c;
        c = shift_n(b, 30);
        b = a;
        a = tmp;
    }

    for (size_t i = 40; i < 60; ++i)
    {
        uint32_t tmp = shift_n(a, 5) + f_2(b, c, d) + e + words[i] + k_2;
        e = d;
        d = c;
        c = shift_n(b, 30);
        b = a;
        a = tmp;
    }

    for (size_t i = 60; i < 80; ++i)
    {
        uint32_t tmp = shift_n(a, 5) + f_3(b, c, d) + e + words[i] + k_3;
        e = d;
        d = c;
        c = shift_n(b, 30);
        b = a;
        a = tmp;
    }

    h_0 += a;
    h_1 += b;
    h_2 += c;
    h_3 += d;
    h_4 += e;
}

uint64_t file_size(const char* const filename)
{
    FILE *f = fopen(filename, "rb");
    uint64_t text_len = 0;

    if (f != NULL)
    {
        fseek(f, 0L, SEEK_END);
        text_len = ftell(f);
        fclose(f);
    }

    return text_len * 8;
}

void print_uint8_t(const uint8_t *arr, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
        printf("%x", arr[i]);
    printf("\n\n");
}

void padding(uint8_t block[64], const uint64_t size_in_bits, bool extra)
{
    uint64_t size_in_bytes = size_in_bits / 8;
    uint64_t size_of_block = size_in_bytes % 64;

    block[size_of_block] = 0b10000000;

    for (size_t i = size_of_block + 1; i < 64; ++i)
        block[i] = 0;

    if (!extra)
    {
        block[56] = 0b11111111 & (size_in_bits >> 56);
        block[57] = 0b11111111 & (size_in_bits >> 48);
        block[58] = 0b11111111 & (size_in_bits >> 40);
        block[59] = 0b11111111 & (size_in_bits >> 32);
        block[60] = 0b11111111 & (size_in_bits >> 24);
        block[61] = 0b11111111 & (size_in_bits >> 16);
        block[62] = 0b11111111 & (size_in_bits >> 8);
        block[63] = 0b11111111 & (size_in_bits);
    }
}
