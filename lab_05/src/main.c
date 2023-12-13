#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CHARS_NUM       256
#define MAX_TREE_HEIGHT 256

unsigned char arr[CHARS_NUM] = {0};
int freq[CHARS_NUM]          = {0};

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

size_t get_file_size(const char *const filename)
{
	FILE *f = fopen(filename, "rb");

	fseek(f, 0L, SEEK_END);
	size_t size = ftell(f);

	fclose(f);

	return size;
}

struct min_heap_node* new_node(unsigned char data, unsigned freq) 
{ 
	struct min_heap_node* temp = (struct min_heap_node *)malloc(sizeof(struct min_heap_node)); 

	temp->left = temp->right = NULL;
	temp->data = data; 
	temp->freq = freq; 

	return temp; 
} 

struct min_heap* create_min_heap(unsigned capacity) 
{ 
	struct min_heap* minHeap = (struct min_heap*)malloc(sizeof(struct min_heap)); 

	minHeap->size = 0; 
	minHeap->capacity = capacity; 
	minHeap->array = (struct min_heap_node **)malloc(minHeap->capacity * 
		sizeof(struct min_heap_node*));

	return minHeap; 
} 

void swap_min_heap_node(struct min_heap_node** a, 
					    struct min_heap_node** b) 
{ 
	struct min_heap_node* t = *a; 
	*a = *b; 
	*b = t; 
} 

void min_heapify(struct min_heap* minHeap, int idx) 
{ 
	int smallest = idx; 
	int left = 2 * idx + 1; 
	int right = 2 * idx + 2; 

	if (left < minHeap->size 
		&& minHeap->array[left]->freq 
			< minHeap->array[smallest]->freq) 
		smallest = left; 

	if (right < minHeap->size 
		&& minHeap->array[right]->freq 
			< minHeap->array[smallest]->freq) 
		smallest = right; 

	if (smallest != idx) { 
		swap_min_heap_node(&minHeap->array[smallest], 
						&minHeap->array[idx]); 
		min_heapify(minHeap, smallest); 
	} 
} 

int is_size_one(struct min_heap* minHeap) 
{ 
	return minHeap->size == 1; 
} 

struct min_heap_node* extract_min(struct min_heap* minHeap) 
{ 
	struct min_heap_node* temp = minHeap->array[0]; 
	minHeap->array[0] = minHeap->array[minHeap->size - 1]; 
	--minHeap->size; 
	min_heapify(minHeap, 0); 

	return temp; 
}

void insert_min_heap(struct min_heap* minHeap, 
				struct min_heap_node* minHeapNode) 
{ 
	++minHeap->size; 
	int i = minHeap->size - 1; 

	while (i 
		&& minHeapNode->freq 
				< minHeap->array[(i - 1) / 2]->freq)
	{ 
		minHeap->array[i] = minHeap->array[(i - 1) / 2]; 
		i = (i - 1) / 2; 
	} 

	minHeap->array[i] = minHeapNode; 
} 

void _build_min_heap(struct min_heap* minHeap) 
{ 
	int n = minHeap->size - 1; 
	int i; 

	for (i = (n - 1) / 2; i >= 0; --i) 
		min_heapify(minHeap, i); 
} 

void fprint_array(int arr[], int n, FILE *f) 
{
	for (size_t i = 0; i < n; ++i) 
		fprintf(f, "%d", arr[i]); 
} 

int is_leaf(struct min_heap_node* root) 
{ 
	return !(root->left) && !(root->right); 
} 

struct min_heap* build_min_heap(unsigned char data[], 
									int freq[], int size) 
{
	struct min_heap* minHeap = create_min_heap(size); 

	for (int i = 0; i < size; ++i) 
		minHeap->array[i] = new_node(data[i], freq[i]); 

	minHeap->size = size; 
	_build_min_heap(minHeap); 

	return minHeap; 
} 

struct min_heap_node* build_huffman_tree(unsigned char data[], 
									int freq[], int size) 

{ 
	struct min_heap_node *left, *right, *top; 

	struct min_heap* minHeap 
		= build_min_heap(data, freq, size); 

	while (!is_size_one(minHeap))
	{ 
		left = extract_min(minHeap); 
		right = extract_min(minHeap); 

		top = new_node('$', left->freq + right->freq);

		top->left = left; 
		top->right = right; 

		insert_min_heap(minHeap, top); 
	} 

	return extract_min(minHeap); 
}

void print_codes(struct min_heap_node* root, int *arr, int top, FILE *f) 
{
	if (root->left)
    { 
		arr[top] = 0; 
		print_codes(root->left, arr, top + 1, f); 
	} 

	if (root->right)
    { 
		arr[top] = 1; 
		print_codes(root->right, arr, top + 1, f); 
	} 

	if (is_leaf(root))
    { 
		fprintf(f, "%u,", root->data);
		fprint_array(arr, top, f);
		fprintf(f, "\n");
	}
}

void find_code(struct min_heap_node* root, int *arr, int top, unsigned char chr, FILE *f) 
{
	if (is_leaf(root))
    { 
		if (root->data == chr)
			fprint_array(arr, top, f);
	}

	if (root->left)
    { 
		arr[top] = 0; 
		find_code(root->left, arr, top + 1, chr, f); 
	} 

	if (root->right)
    { 
		arr[top] = 1; 
		find_code(root->right, arr, top + 1, chr, f); 
	}
}

struct min_heap_node* huffman_compression(char data[], int freq[], int size) 
{ 
	struct min_heap_node* root 
		= build_huffman_tree(data, freq, size); 

	int arr[MAX_TREE_HEIGHT], top = 0;

    FILE *f = fopen("tree.txt", "w");
	print_codes(root, arr, top, f);
    fclose(f);

    return root;
}

void encode_file(struct min_heap_node* root, const char *const filename_in, 
										   const char *const filename_out)
{
    FILE *f_in = fopen(filename_in, "rb");
	FILE *f_out = fopen(filename_out, "w");

	unsigned char chr;
	int arr[MAX_TREE_HEIGHT], top = 0;

	while (fread(&chr, sizeof(unsigned char), 1, f_in) == 1)
		find_code(root, arr, top, chr, f_out);
    
    fclose(f_in);
	fclose(f_out);
}

void swap_uc(unsigned char* xp, unsigned char* yp)
{
    unsigned char temp = *xp;
    *xp = *yp;
    *yp = temp;
}

void swap_int(int* xp, int* yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}
 
void bubble_sort(unsigned char *arr1, int *arr2, const size_t size)
{
    size_t i, j;
    int swapped;

    for (i = 0; i < size - 1; i++)
    {
        swapped = 0;

        for (j = 0; j < size - i - 1; j++)
        {
            if (freq[j] > freq[j + 1])
            {
                swap_uc(&arr1[j], &arr1[j + 1]);
                swap_int(&arr2[j], &arr2[j + 1]);
                swapped = 1;
            }
        }
 
        if (swapped == 0)
            break;
    }
}

size_t new_base(int *freq, const size_t size)
{
    for (size_t i = size - 1; i != 0; --i)
        if (freq[i] == 0)
            return i + 1;

    return 0;
}

// для разжатия файла в представлении 0-й и 1-ц
void decode_file(struct min_heap_node* root, const char *const filename_in,
										   const char *const filename_out)
{
    FILE *f_in = fopen(filename_in, "r");
	FILE *f_out = fopen(filename_out, "wb");
	
	char bit;
    struct min_heap_node* curr = root;

    while (fread(&bit, sizeof(char), 1, f_in) == 1)
    {
        if (bit == '0')
            curr = curr->left;
        else if (bit == '1')
            curr = curr->right;
 
        if (curr->left == NULL && curr->right == NULL)
        {
            fwrite(&curr->data, sizeof(unsigned char), 1, f_out);
            curr = root;
        }
    }
    
    fclose(f_in);
	fclose(f_out);
}

// для разжатия файла в байтовом представлении
void decode_byte_file(struct min_heap_node* root, const char *const filename_in,
										   const char *const filename_out)
{
	size_t size = get_file_size(filename_in) - 1;

    FILE *f_in = fopen(filename_in, "rb");
	FILE *f_out = fopen(filename_out, "wb");
	
	unsigned char byte;
    struct min_heap_node* curr = root;
	uint8_t last_bits;
	int shifts = 8;
	
	fread(&last_bits, sizeof(uint8_t), 1, f_in);

    while (fread(&byte, sizeof(unsigned char), 1, f_in) > 0)
    {
		if (size == 1)
			shifts = last_bits;

		for (size_t i = 0; i < shifts; ++i)
		{
			if (byte & (1 << (8 - i - 1)))
				curr = curr->right;
        	else
				curr = curr->left;

			if (curr->left == NULL && curr->right == NULL)
			{
				fwrite(&curr->data, sizeof(unsigned char), 1, f_out);
				curr = root;
			}
		}
		
		--size;
    }
    
    fclose(f_in);
	fclose(f_out);
}

void null_buffer(unsigned char *buffer, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
        buffer[i] = '0';
}

uint8_t bits_to_byte(unsigned char *buffer, const size_t size)
{
    uint8_t res = 0b00000000;
    uint8_t mask = 0b10000000;

    for (size_t i = 0; i < size; ++i, mask >>= 1)
        if (buffer[i] == '1')
            res += mask;

    return res;
}

void bits_stream_to_file(const char *const filename_in,
						 const char *const filename_out)
{
	size_t size_in_bits = get_file_size(filename_in);
	uint8_t size = size_in_bits % 8;

	FILE *f_in = fopen(filename_in, "r");
	FILE *f_out = fopen(filename_out, "wb");

	uint8_t byte = 0;
	unsigned char bits[8];

	null_buffer(bits, 8);

	fwrite(&size, sizeof(uint8_t), 1, f_out);

	while (fread(&bits, sizeof(char), 8, f_in) > 0)
	{
		byte = bits_to_byte(bits, 8);
		fwrite(&byte, sizeof(uint8_t), 1, f_out);
		null_buffer(bits, 8);
	}

	fclose(f_in);
	fclose(f_out);
}

void print_ratio(const char *const filename_original,
				 const char *const filename_compressed)
{
	FILE *f_original = fopen(filename_original, "rb");
	FILE *f_compressed = fopen(filename_compressed, "rb");

	fseek(f_original, 0L, SEEK_END);
	size_t size_original = ftell(f_original);
	fseek(f_compressed, 0L, SEEK_END);
	size_t size_compressed = ftell(f_compressed);

	fclose(f_original);
	fclose(f_compressed);

	printf("Compression ratio: %llf.\n", (double)size_original / (double)size_compressed);
}

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("Error: program requires 3 parameters.\n");
		return 1;
	}

	if (get_file_size(argv[1]) == 0)
	{
		printf("Error: input file is empty.\n");
		return 2;
	}

	for (size_t i = 0; i < CHARS_NUM; ++i)
	{
		arr[i] = i;
		freq[i] = 0;
	}

    FILE *f = fopen(argv[1], "rb");
    unsigned char chr;

    while (fread(&chr, sizeof(unsigned char), 1, f) == 1)
        freq[chr]++;

    fclose(f);

    bubble_sort(arr, freq, CHARS_NUM);
    size_t new_start = new_base(freq, CHARS_NUM);

	struct min_heap_node* root = huffman_compression(arr + new_start, freq + new_start, CHARS_NUM - new_start);

	encode_file(root, argv[1], "tmp.txt");
	bits_stream_to_file("tmp.txt", argv[2]);
	decode_byte_file(root, argv[2], argv[3]);

	print_ratio(argv[1], argv[2]);

	return 0; 
}
