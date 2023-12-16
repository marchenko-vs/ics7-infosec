#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_

#define CHARS_NUM       256
#define MAX_TREE_HEIGHT 256

extern unsigned char arr[CHARS_NUM];
extern int freq[CHARS_NUM];

struct min_heap_node
{ 
	unsigned char data; 
	unsigned      freq; 

	struct min_heap_node *left;
	struct min_heap_node *right;
};

struct min_heap
{ 
	unsigned size; 
	unsigned capacity; 

	struct min_heap_node **array; 
};

size_t get_file_size(const char *const filename);
struct min_heap_node* build_huffman_tree(unsigned char data[], 
    int freq[], int size);
void encode_file(struct min_heap_node* root, const char *const filename_in, 
    const char *const filename_out);
void bubble_sort(unsigned char *arr1, int *arr2, const size_t size);
size_t new_base(int *freq, const size_t size);
void decode_file(struct min_heap_node* root, const char *const filename_in,
    const char *const filename_out);
void decode_byte_file(struct min_heap_node* root, const char *const filename_in,
    const char *const filename_out);
void load_frequencies(const char *const filename);
void save_frequencies(const char *const filename);
void print_ratio(const char *const filename_original,
    const char *const filename_compressed);
void bits_stream_to_file(const char *const filename_in,
    const char *const filename_out);
struct min_heap_node* huffman_compression(unsigned char data[], int freq[], int size);

#endif // _HUFFMAN_H_
