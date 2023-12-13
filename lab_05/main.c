#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

struct MinHeapNode
{ 
	unsigned char data; 
	unsigned freq; 
	int leaf;

	struct MinHeapNode *left, *right;
};

struct MinHeap
{ 
	unsigned size; 
	unsigned capacity; 

	struct MinHeapNode** array; 
};

unsigned char arr[256] = {0};
int freq[256]          = {0};

struct MinHeapNode* newNode(unsigned char data, unsigned freq) 
{ 
	struct MinHeapNode* temp = (struct MinHeapNode*)malloc( 
		sizeof(struct MinHeapNode)); 

	temp->left = temp->right = NULL;
	temp->leaf = 1;
	temp->data = data; 
	temp->freq = freq; 

	return temp; 
} 

struct MinHeap* createMinHeap(unsigned capacity) 
{ 
	struct MinHeap* minHeap 
		= (struct MinHeap*)malloc(sizeof(struct MinHeap)); 

	minHeap->size = 0; 
	minHeap->capacity = capacity; 
	minHeap->array = (struct MinHeapNode**)malloc( 
		minHeap->capacity * sizeof(struct MinHeapNode*));

	return minHeap; 
} 

void swapMinHeapNode(struct MinHeapNode** a, 
					struct MinHeapNode** b) 
{ 
	struct MinHeapNode* t = *a; 
	*a = *b; 
	*b = t; 
} 

void minHeapify(struct MinHeap* minHeap, int idx) 
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
		swapMinHeapNode(&minHeap->array[smallest], 
						&minHeap->array[idx]); 
		minHeapify(minHeap, smallest); 
	} 
} 

int isSizeOne(struct MinHeap* minHeap) 
{ 
	return minHeap->size == 1; 
} 

struct MinHeapNode* extractMin(struct MinHeap* minHeap) 
{ 
	struct MinHeapNode* temp = minHeap->array[0]; 
	minHeap->array[0] = minHeap->array[minHeap->size - 1]; 
	--minHeap->size; 
	minHeapify(minHeap, 0); 

	return temp; 
}

void insertMinHeap(struct MinHeap* minHeap, 
				struct MinHeapNode* minHeapNode) 
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

void buildMinHeap(struct MinHeap* minHeap) 
{ 
	int n = minHeap->size - 1; 
	int i; 

	for (i = (n - 1) / 2; i >= 0; --i) 
		minHeapify(minHeap, i); 
} 

void printArr(int arr[], int n, FILE *f) 
{
	for (size_t i = 0; i < n; ++i) 
		fprintf(f, "%d", arr[i]); 
} 

int isLeaf(struct MinHeapNode* root) 
{ 
	return !(root->left) && !(root->right); 
} 

struct MinHeap* createAndBuildMinHeap(unsigned char data[], 
									int freq[], int size) 
{
	struct MinHeap* minHeap = createMinHeap(size); 

	for (int i = 0; i < size; ++i) 
		minHeap->array[i] = newNode(data[i], freq[i]); 

	minHeap->size = size; 
	buildMinHeap(minHeap); 

	return minHeap; 
} 

struct MinHeapNode* buildHuffmanTree(unsigned char data[], 
									int freq[], int size) 

{ 
	struct MinHeapNode *left, *right, *top; 

	struct MinHeap* minHeap 
		= createAndBuildMinHeap(data, freq, size); 

	while (!isSizeOne(minHeap))
	{ 
		left = extractMin(minHeap); 
		right = extractMin(minHeap); 

		top = newNode('$', left->freq + right->freq);
		top->leaf = 0;

		top->left = left; 
		top->right = right; 

		insertMinHeap(minHeap, top); 
	} 

	return extractMin(minHeap); 
} 

void printCodes(struct MinHeapNode* root, int *arr, int top, FILE *f) 
{
	if (root->left)
    { 
		arr[top] = 0; 
		printCodes(root->left, arr, top + 1, f); 
	} 

	if (root->right)
    { 
		arr[top] = 1; 
		printCodes(root->right, arr, top + 1, f); 
	} 

	if (isLeaf(root))
    { 
		fprintf(f, "%u,", root->data);
		printArr(arr, top, f);
		fprintf(f, "\n");
	}
}

void find_code(struct MinHeapNode* root, int *arr, int top, unsigned char chr, FILE *f) 
{
	if (isLeaf(root))
    { 
		if (root->data == chr)
			printArr(arr, top, f);
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

struct MinHeapNode* build_huffman_tree(char data[], int freq[], int size) 
{ 
	struct MinHeapNode* root 
		= buildHuffmanTree(data, freq, size); 

	int arr[MAX_TREE_HT], top = 0;

    FILE *f = fopen("tree.csv", "w");
	printCodes(root, arr, top, f);
    fclose(f);

    return root;
}

void encode_file(struct MinHeapNode* root, const char *const filename_in, 
										   const char *const filename_out)
{
    FILE *f_in = fopen(filename_in, "rb");
	FILE *f_out = fopen(filename_out, "w");

	unsigned char chr;
	int arr[MAX_TREE_HT], top = 0;

	while (fread(&chr, sizeof(unsigned char), 1, f_in) == 1)
	{
		find_code(root, arr, top, chr, f_out);
	}
    
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

void decode_file(struct MinHeapNode* root, const char *const filename_in,
										   const char *const filename_out)
{
    FILE *f_in = fopen(filename_in, "r");
	FILE *f_out = fopen(filename_out, "wb");
	
	char bit;
    struct MinHeapNode* curr = root;

    while (fread(&bit, sizeof(char), 1, f_in) == 1)
    {
        if (bit == '0')
            curr = curr->left;
        else if ((bit == '1'))
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

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("Error: 3 filenames are required.\n");
		return 1;
	}

	for (size_t i = 0; i < 256; ++i)
	{
		arr[i] = i;
		freq[i] = 0;
	}

    FILE *f = fopen(argv[1], "rb");
    unsigned char chr;

    while (fread(&chr, sizeof(unsigned char), 1, f) == 1)
        freq[chr]++;

    fclose(f);

    bubble_sort(arr, freq, 256);
    size_t new_start = new_base(freq, 256);

	struct MinHeapNode* root = build_huffman_tree(arr + new_start, freq + new_start, 256 - new_start);

	encode_file(root, argv[1], argv[2]);
    decode_file(root, argv[2], argv[3]);

	return 0; 
}
