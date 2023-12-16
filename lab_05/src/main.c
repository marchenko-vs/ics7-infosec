#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "huffman.h"

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("Error: program requires 3 parameters.\n");
		return 1;
	}

	if (get_file_size(argv[2]) == 0)
	{
		printf("Error: input file is empty.\n");
		return 2;
	}

	for (size_t i = 0; i < CHARS_NUM; ++i)
	{
		arr[i] = i;
		freq[i] = 0;
	}

	if (strcmp(argv[1], "-c") == 0)
	{
		FILE *f = fopen(argv[2], "rb");
		unsigned char chr;

		while (fread(&chr, sizeof(unsigned char), 1, f) == 1)
			freq[chr]++;

		fclose(f);

		bubble_sort(arr, freq, CHARS_NUM);
		size_t new_start = new_base(freq, CHARS_NUM);

		save_frequencies("cfg/freq.csv");

		struct min_heap_node* root = huffman_compression(arr + new_start, freq + new_start, CHARS_NUM - new_start);

		encode_file(root, argv[2], "cfg/tmp.txt");
		bits_stream_to_file("cfg/tmp.txt", argv[3]);
		print_ratio(argv[2], argv[3]);
	}
	else if (strcmp(argv[1], "-d") == 0)
	{
		load_frequencies("cfg/freq.csv");
		bubble_sort(arr, freq, CHARS_NUM);
		size_t new_start = new_base(freq, CHARS_NUM);

		struct min_heap_node* root = huffman_compression(arr + new_start, freq + new_start, CHARS_NUM - new_start);

		decode_byte_file(root, argv[2], argv[3]);
	}
	else
	{
		printf("Error: incorrect option.\n");
		return 3;
	}

	return 0; 
}
