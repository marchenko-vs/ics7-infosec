#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define CHARS_NUM 256

typedef struct
{
    uint8_t chr;
    size_t repeats;
} data_t;

struct node
{
    data_t data;
    int value;

    struct node *left;
    struct node *right;
};

typedef struct node node_t;

node_t *new_node(data_t data)
{
    node_t *temp = malloc(sizeof(node_t));
    temp->data = data;
    temp->left = temp->right = NULL;

    return temp;
}

int cmp_data(const void *const a, const void *const b)
{
    return ((data_t *)a)->repeats - ((data_t *)b)->repeats;
}

node_t *insert(node_t *node, data_t data)
{
    if (node == NULL)
        return new_node(data);
 
    if (cmp_data(&data, &node->data) < 0)
        node->left = insert(node->left, data);
    else
        node->right = insert(node->right, data);
 
    return node;
}

void inorder(node_t *root)
{
    if (root != NULL)
    {
        inorder(root->left);
        printf("%d   : %lld\n", root->data.chr, root->data.repeats);
        inorder(root->right);
    }
}
	
void free_tree(node_t *root)
{
    while (1)
    {       
        if (root == NULL)
            break;
        else if (root->left != NULL)
        {
            free_tree(root->left);
            root->left = NULL;
        }
        else if (root->right != NULL)
        {
            free_tree(root->right);
            root->right = NULL;
        }
        else
        { 
            free(root);
            return;
        }
    }
}

void array_insert(data_t *array, const size_t size, data_t item, size_t pos)
{
    for (size_t i = size - 1; i >= pos && i != 0; i--)
        array[i] = array[i - 1];
 
    array[pos - 1] = item;
}

size_t array_find_start(data_t *array, const size_t size)
{
    for (size_t i = size - 1; i != 0; --i)
        if (array[i].repeats == 0)
            return i + 1;

    return 0;
}

int main(int argc, char **argv)
{
    FILE *f = fopen(argv[1], "rb");
    data_t frequencies[CHARS_NUM] = {0};
    uint8_t chr;
    while (fread(&chr, sizeof(uint8_t), 1, f) == 1)
    {
        frequencies[chr].chr = chr;
        ++frequencies[chr].repeats;
    }
    fclose(f);

    qsort(&frequencies[0], CHARS_NUM, sizeof(data_t), cmp_data);

    size_t start_index = array_find_start(frequencies, CHARS_NUM);
    // size_t new_size = CHARS_NUM - start_index;

    data_t data_1 = { 0, frequencies[start_index].repeats + frequencies[start_index + 1].repeats };

    node_t *root = NULL;
    root = insert(root, data_1);
    node_t *left = new_node(frequencies[start_index]);
    node_t *right = new_node(frequencies[start_index + 1]);
    root->left = left;
    root->right = right;
 
    inorder(root);
    free_tree(root);

    return 0;
}
