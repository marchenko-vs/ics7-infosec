#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

int ip_table[8][8];
int pc_1_table[8][8]; // [8][7]
int pc_2_table[8][8]; // [8][6]
int e_table[8][8]; // [8][6]
int p_table[8][8]; // [8][4]
int ip_reversed_table[8][8];

int s_tables[8][4][16];

int shifts_table[16] = {1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

unsigned char keys[16][49];

unsigned char key[65];
unsigned char iv[65];

int init_table(const char* const filename, int table[8][8], const size_t rows, const size_t cols)
{
    FILE *f;
    int rc = 0;

    if ((f = fopen(filename, "r")) == NULL)
        rc = 1;

    for (size_t i = 0; rc == 0 && i < rows; ++i)
    {
        for (size_t j = 0; rc == 0 && j < cols; ++j)
        {
            if (fscanf(f, "%d", &table[i][j]) != 1)
                rc = 2;
        }
    }

    if (rc != 1)
        fclose(f);

    return rc;
}

int init_s_table(const char* const filename, int table[4][16])
{
    FILE *f;
    int rc = 0;

    if ((f = fopen(filename, "r")) == NULL)
        rc = 1;

    for (size_t i = 0; rc == 0 && i < 4; ++i)
        for (size_t j = 0; rc == 0 && j < 16; ++j)
        {
            if (fscanf(f, "%d", &table[i][j]) != 1)
                rc = 2;
        }

    if (rc != 1)
        fclose(f);

    return rc;
}

void from_64_to_56(const unsigned char* const key_64, unsigned char *key_56)
{
    for (size_t i = 0, k = 0; i < 8; ++i)
    {
        for (size_t j = 0; j < 7; ++j, ++k)
        {
            key_56[k] = key_64[pc_1_table[i][j] - 1];
        }
    }
}

void from_56_to_48(const unsigned char* const text_56, unsigned char *text_48)
{
    for (size_t i = 0, k = 0; i < 8; ++i)
    {
        for (size_t j = 0; j < 6; ++j, ++k)
        {
            text_48[k] = text_56[pc_2_table[i][j] - 1];
        }
    }
}

void from_32_to_48(const unsigned char* const text_32, unsigned char *text_48)
{
    for (size_t i = 0, k = 0; i < 8; ++i)
    {
        for (size_t j = 0; j < 6; ++j, ++k)
        {
            text_48[k] = text_32[e_table[i][j] - 1];
        }
    }
}

void split(const unsigned char* const key, unsigned char *half, const size_t n)
{
    for (size_t i = 0; i < n; ++i)
        half[i] = key[i];
}

void merge(const unsigned char* const buffer_1,
           const unsigned char* const buffer_2,
           char unsigned *merged, const size_t n)
{
    size_t i = 0;

    for (size_t j = 0; j < n / 2; ++i, ++j)
        merged[i] = buffer_1[j];

    for (size_t j = 0; j < n / 2; ++i, ++j)
        merged[i] = buffer_2[j];
}

void shift_left(unsigned char* subkey)
{
    unsigned char first_bit = subkey[0];

    for (size_t i = 0; i < 27; ++i)
        subkey[i] = subkey[i + 1];

    subkey[27] = first_bit;
}

void n_shifts_left(unsigned char* subkey, const size_t n)
{
    for (size_t i = 0; i < n; ++i)
        shift_left(subkey);
}

void permutation(unsigned char *src, unsigned char *permuted,
                 int table[8][8], const size_t rows, const size_t cols)
{
    for (size_t i = 0, k = 0; i < rows; ++i)
    {
        for (size_t j = 0; j < cols; ++j, ++k)
        {
            permuted[k] = src[table[i][j] - 1];
        }
    }
}

void fprint_block(FILE *f, const unsigned char* const buffer)
{
    for (size_t i = 0; i < 8; ++i)
    {
        fprintf(f, "%c", buffer[i]);
    }
}

void reverse(unsigned char* buffer, const size_t size)
{
    for (size_t i = 0; i < size / 2; ++i)
    {
        unsigned char tmp = buffer[i];
        buffer[i] = buffer[size - i - 1];
        buffer[size - i - 1] = tmp;
    }
}

void dec_to_bin(int byte, unsigned char *bits)
{
    if (byte == 0)
    {
        for (size_t i = 0; i < 8; ++i)
        {
            bits[i] = '0';
        }

        return;
    }

    size_t i = 0;

    while (byte > 0)
    {
        int mod = byte % 2;
        if (mod == 1)
            bits[i++] = '1';
        else
            bits[i++] = '0';
        byte /= 2;
    }

    for ( ; i < 8; ++i)
        bits[i] = '0';

    reverse(bits, 8);
}

int bin_to_dec(unsigned char *bits, const size_t size)
{
    int degree = 0, result = 0;

    for (ssize_t i = size - 1; i >= 0; --i, ++degree)
    {
        if (bits[i] == '1')
        {
            result += pow(2, degree);
        }
    }

    return result;
}

unsigned char _xor(const unsigned char a, const unsigned char b)
{
    if ((a == '1' && b == '0') || (a == '0' && b == '1'))
        return '1';
    
    return '0';
}

void xor_buffers(const unsigned char* const buffer_1,
         const unsigned char* const buffer_2,
         unsigned char* result, const size_t len)
{
    for (size_t i = 0; i < len; ++i)
    {
        result[i] = _xor(buffer_1[i], buffer_2[i]);
    }
}

void copy(const unsigned char* const src,
          unsigned char* dst, const size_t len)
{
    for (size_t i = 0; i < len; ++i)
        dst[i] = src[i];
}

void init_tables()
{
    init_table("cfg/ip.txt", ip_table, 8, 8);
    init_table("cfg/pc-1.txt", pc_1_table, 8, 7);
    init_table("cfg/pc-2.txt", pc_2_table, 8, 6);
    init_table("cfg/e.txt", e_table, 8, 6);
    init_table("cfg/p.txt", p_table, 8, 4);
    init_table("cfg/ip-reversed.txt", ip_reversed_table, 8, 8);

    init_s_table("cfg/s-1.txt", s_tables[0]);
    init_s_table("cfg/s-2.txt", s_tables[1]);
    init_s_table("cfg/s-3.txt", s_tables[2]);
    init_s_table("cfg/s-4.txt", s_tables[3]);
    init_s_table("cfg/s-5.txt", s_tables[4]);
    init_s_table("cfg/s-6.txt", s_tables[5]);
    init_s_table("cfg/s-7.txt", s_tables[6]);
    init_s_table("cfg/s-8.txt", s_tables[7]);
}

void generate_keys(unsigned char *key)
{
    unsigned char key_permuted[57];
    key_permuted[56] = '\0';
    from_64_to_56(key, key_permuted);

    unsigned char left_key[29];
    left_key[28] = '\0';
    unsigned char right_key[29];
    right_key[28] = '\0';
    split(key_permuted, left_key, 28);
    split(key_permuted + 28, right_key, 28);

    for (size_t i = 0; i < 16; ++i)
    {
        n_shifts_left(left_key, shifts_table[i]);
        n_shifts_left(right_key, shifts_table[i]);
        unsigned char key_merged[57];
        key_merged[56] = '\0';
        merge(left_key, right_key, key_merged, 56);
        from_56_to_48(key_merged, keys[i]);
        keys[i][48] = '\0';
    }
}

void encrypt_block(unsigned char *plaintext, unsigned char *key, unsigned char *encrypted_text)
{
    generate_keys(key);

    // plaintext permutation
    unsigned char plaintext_permuted[65];
    plaintext_permuted[64] = '\0';
    permutation(plaintext, plaintext_permuted, ip_table, 8, 8);

    unsigned char left_plaintext[33];
    left_plaintext[32] = '\0';
    unsigned char right_plaintext[33];
    right_plaintext[32] = '\0';

    // divide initial plaintext into 2 subtexts
    split(plaintext_permuted, left_plaintext, 32);
    split(plaintext_permuted + 32, right_plaintext, 32);

    unsigned char encrypted[65];
    encrypted[64] = '\0';

    unsigned char current_left[33];
    current_left[32] = '\0';
    unsigned char current_right[33];
    current_right[32] = '\0';
    unsigned char prev_left[33];
    prev_left[32] = '\0';
    unsigned char prev_right[33];
    prev_right[32] = '\0';

    copy(left_plaintext, prev_left, 32);
    copy(right_plaintext, prev_right, 32);

    for (size_t i = 0; i < 16; ++i)
    {
        copy(prev_right, current_left, 32);

        unsigned char s_merged_permuted_1[33];
        s_merged_permuted_1[32] = '\0';

        unsigned char expanded[49];
        expanded[48] = '\0';
        unsigned char result[49];
        result[48] = '\0';
        from_32_to_48(prev_right, expanded);
        xor_buffers(keys[i], expanded, result, 48);

        unsigned char s_buf[8][7];
        unsigned char s_merged[33];
        s_merged[32] = '\0';

        for (size_t i = 0, j = 0, k = 0; i < 8; ++i, j += 6, k += 4)
        {
            s_buf[i][6] = '\0';
            split(result + j, s_buf[i], 6);
            
            unsigned char row[3];
            row[0] = s_buf[i][0];
            row[1] = s_buf[i][5];
            row[2] = '\0';
            int row_index = bin_to_dec(row, 2);

            unsigned char column[5];
            column[0] = s_buf[i][1];
            column[1] = s_buf[i][2];
            column[2] = s_buf[i][3];
            column[3] = s_buf[i][4];
            column[4] = '\0';
            int column_index = bin_to_dec(column, 4);

            unsigned char index[9];
            index[8] = '\0';
            dec_to_bin(s_tables[i][row_index][column_index], index);
            index[8] = '\0';

            copy(index + 4, s_merged + k, 4);
        }

        permutation(s_merged, s_merged_permuted_1, p_table, 8, 4);
        xor_buffers(prev_left, s_merged_permuted_1, current_right, 32);

        copy(current_left, prev_left, 32);
        copy(current_right, prev_right, 32);
    }

    merge(current_right, current_left, encrypted, 64);
    permutation(encrypted, encrypted_text, ip_reversed_table, 8, 8);
}

void decrypt_block(unsigned char *plaintext, unsigned char *key, unsigned char *decrypted_text)
{
    generate_keys(key);

    // plaintext permutation
    unsigned char plaintext_permuted[65];
    plaintext_permuted[64] = '\0';
    permutation(plaintext, plaintext_permuted, ip_table, 8, 8);

    unsigned char left_plaintext[33];
    left_plaintext[32] = '\0';
    unsigned char right_plaintext[33];
    right_plaintext[32] = '\0';

    // divide initial plaintext into 2 subtexts
    split(plaintext_permuted, left_plaintext, 32);
    split(plaintext_permuted + 32, right_plaintext, 32);

    unsigned char encrypted[65];
    encrypted[64] = '\0';

    unsigned char current_left[33];
    current_left[32] = '\0';
    unsigned char current_right[33];
    current_right[32] = '\0';
    unsigned char prev_left[33];
    prev_left[32] = '\0';
    unsigned char prev_right[33];
    prev_right[32] = '\0';

    copy(left_plaintext, prev_left, 32);
    copy(right_plaintext, prev_right, 32);

    for (size_t i = 0; i < 16; ++i)
    {
        copy(prev_right, current_left, 32);

        unsigned char s_merged_permuted_1[33];
        s_merged_permuted_1[32] = '\0';

        unsigned char expanded[49];
        expanded[48] = '\0';
        unsigned char result[49];
        result[48] = '\0';
        from_32_to_48(prev_right, expanded);
        xor_buffers(keys[15 - i], expanded, result, 48);

        unsigned char s_buf[8][7];
        unsigned char s_merged[33];
        s_merged[32] = '\0';

        for (size_t i = 0, j = 0, k = 0; i < 8; ++i, j += 6, k += 4)
        {
            s_buf[i][6] = '\0';
            split(result + j, s_buf[i], 6);
            
            unsigned char row[3];
            row[0] = s_buf[i][0];
            row[1] = s_buf[i][5];
            row[2] = '\0';
            int row_index = bin_to_dec(row, 2);

            unsigned char column[5];
            column[0] = s_buf[i][1];
            column[1] = s_buf[i][2];
            column[2] = s_buf[i][3];
            column[3] = s_buf[i][4];
            column[4] = '\0';
            int column_index = bin_to_dec(column, 4);

            unsigned char index[9];
            index[8] = '\0';
            dec_to_bin(s_tables[i][row_index][column_index], index);
            index[8] = '\0';

            copy(index + 4, s_merged + k, 4);
        }

        permutation(s_merged, s_merged_permuted_1, p_table, 8, 4);
        xor_buffers(prev_left, s_merged_permuted_1, current_right, 32);

        copy(current_left, prev_left, 32);
        copy(current_right, prev_right, 32);
    }

    merge(current_right, current_left, encrypted, 64);
    permutation(encrypted, decrypted_text, ip_reversed_table, 8, 8);
}

void fwrite_text(const char* const filename, unsigned char* buffer, const size_t size)
{
    FILE* f;

    if ((f = fopen(filename, "wb")) == NULL)
        return;

    for (size_t i = 0; i < size; ++i)
        fwrite(&buffer[i], sizeof(unsigned char), 1, f);

    fclose(f);
}

void fread_key(const char* const filename, unsigned char* buffer)
{
    FILE* f = fopen(filename, "r");
    fscanf(f, "%s", buffer);
    fclose(f);
}

void fill_buffer(unsigned char *buffer, const size_t size)
{
    for (size_t i = size; i < 8; ++i)
    {
        buffer[i] = 0;
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
    {
        printf("Error: program requires 3 filenames.\n");
        return -1;
    }

    fread_key("cfg/key.txt", key);
    fread_key("cfg/iv.txt", iv);

    init_tables();

    unsigned char plain_block[9] = "";
    unsigned char cipher_block[9] = "";

    unsigned char plain_bits[65] = "";
    unsigned char cipher_bits[65] = "";
    unsigned char plain_xor_cipher[65] = "";
    unsigned char cipher_iv[65] = "";
    unsigned char bits[9] = "";

    FILE *f_in = fopen(argv[2], "rb");
    FILE *f_out = fopen(argv[3], "wb");

    size_t progress = 0;

    if (strcmp(argv[1], "-e") == 0)
    {
        size_t len = fread(plain_block, sizeof(unsigned char), 8, f_in);

        if (len == 0)
        {
            printf("Error: empty input file.\n");
            fclose(f_in);
            fclose(f_out);
            return -2;
        }

        if (len < 8)
            fill_buffer(plain_block, len);

        for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
        {
            dec_to_bin(plain_block[i], bits);
            copy(bits, plain_bits + j, 8);
        }

        xor_buffers(plain_bits, iv, cipher_iv, 64);
        encrypt_block(cipher_iv, key, cipher_bits);

        for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
        {
            copy(cipher_bits + j, bits, 8);
            cipher_block[i] = bin_to_dec(bits, 8);
        }

        fwrite(cipher_block, sizeof(unsigned char), 8, f_out);
        xor_buffers(plain_bits, cipher_bits, plain_xor_cipher, 64);
        len = fread(plain_block, sizeof(unsigned char), 8, f_in);

        while (len != 0)
        {
            if (len < 8)
                fill_buffer(plain_block, len);

            for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
            {
                dec_to_bin(plain_block[i], bits);
                copy(bits, plain_bits + j, 8);
            }

            xor_buffers(plain_bits, plain_xor_cipher, cipher_iv, 64);
            encrypt_block(cipher_iv, key, cipher_bits);
            
            for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
            {
                copy(cipher_bits + j, bits, 8);
                cipher_block[i] = bin_to_dec(bits, 8);
            }

            fwrite(cipher_block, sizeof(unsigned char), 8, f_out);

            progress++;
            printf("Iteration: %zu\n", progress);

            xor_buffers(plain_bits, cipher_bits, plain_xor_cipher, 64);
            len = fread(plain_block, sizeof(unsigned char), 8, f_in);
        }
    }
    else if (strcmp(argv[1], "-d") == 0)
    {
        size_t len = fread(cipher_block, sizeof(unsigned char), 8, f_in);

        if (len == 0)
        {
            printf("Error: empty input file.\n");
            fclose(f_in);
            fclose(f_out);
            return -2;
        }

        if (len < 8)
            fill_buffer(cipher_block, len);

        for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
        {
            dec_to_bin(cipher_block[i], bits);
            copy(bits, cipher_bits + j, 8);
        }

        decrypt_block(cipher_bits, key, cipher_iv);
        xor_buffers(iv, cipher_iv, plain_bits, 64);

        for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
        {
            copy(plain_bits + j, bits, 8);
            plain_block[i] = bin_to_dec(bits, 8);
        }

        fwrite(plain_block, sizeof(unsigned char), 8, f_out);
        xor_buffers(cipher_bits, plain_bits, plain_xor_cipher, 64);
        len = fread(cipher_block, sizeof(unsigned char), 8, f_in);

        while (len != 0)
        {
            if (len < 8)
                fill_buffer(cipher_block, len);

            for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
            {
                dec_to_bin(cipher_block[i], bits);
                copy(bits, cipher_bits + j, 8);
            }

            decrypt_block(cipher_bits, key, cipher_iv);
            xor_buffers(cipher_iv, plain_xor_cipher, plain_bits, 64);
            
            for (size_t i = 0, j = 0; i < 8; ++i, j += 8)
            {
                copy(plain_bits + j, bits, 8);
                plain_block[i] = bin_to_dec(bits, 8);
            }

            fwrite(plain_block, sizeof(unsigned char), 8, f_out);
            
            progress++;
            printf("Iteration: %zu\n", progress);

            xor_buffers(cipher_bits, plain_bits, plain_xor_cipher, 64);
            len = fread(cipher_block, sizeof(unsigned char), 8, f_in);
        }
    }
    else
    {
        printf("Error: incorrect option %s.\n", argv[1]);
        return -3;
    }

    fclose(f_in);
    fclose(f_out);

    return 0;
}
