/**
 * HW7, Cи продвинутый. гр.Д01-134 Попов В.Г
 * D1: В программе описана структура для хранения двоичного дерева:
    typedef struct tree {
    datatype key;
    struct tree *left, *right;
    } tree;
 * Необходимо реализовать функцию, которая печатает вид дерева сверху. 
 * Функция должна строго соответствовать прототипу:
    void btUpView(tree *root)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree 
{
    int key;
    struct tree *left, *right, *parent;
} tree;

void insert(tree **root, int key, tree *pt)  
{ 
    if(!(*root)) 
    {  
        *root = malloc(sizeof(tree)); 
        (*root)->key = key; 
        (*root)->parent = pt;  
        (*root)->left = (*root)->right = NULL;
    } 
    else if(key < (*root)->key) 
        insert(&((*root)->left), key, *root); 
    else 
        insert(&((*root)->right), key, *root); 
}

// Поиск крайних левых и правых вертикальных координат дерева
void findBounds(tree *node, int hd, int *min_hd, int *max_hd) 
{
    if (node == NULL)
        return;

    if (hd < *min_hd) *min_hd = hd;
    if (hd > *max_hd) *max_hd = hd;

    findBounds(node->left, hd - 1, min_hd, max_hd);
    findBounds(node->right, hd + 1, min_hd, max_hd);
}

// Поиск узла на конкретной вертикали (hd), находящегося на минимальной глубине (depth)
void getTopViewNode(tree *node, int hd, int depth, int target_hd, tree **best_node, int *min_depth) 
{
    if (node == NULL)
        return;

    if (hd == target_hd) {
        if (*best_node == NULL || depth < *min_depth) 
        {
            *best_node = node;
            *min_depth = depth;
        }
    }

    getTopViewNode(node->left, hd - 1, depth + 1, target_hd, best_node, min_depth);
    getTopViewNode(node->right, hd + 1, depth + 1, target_hd, best_node, min_depth);
}

void btUpView(tree *root)
{
    if (root == NULL) 
        return; 

    int min_hd = 0;
    int max_hd = 0;

    findBounds(root, 0, &min_hd, &max_hd);

    for (int i = min_hd; i <= max_hd; i++) 
    {
        tree *best_node = NULL;
        int min_depth = 1e9;

        getTopViewNode(root, 0, 0, i, &best_node, &min_depth);

        if (best_node != NULL)
        {
            printf("%d ", best_node->key);
        }
    }
    printf("\n");
}

tree* min_tree(tree *root) 
{ 
    tree *find = root; 
    while(find && find->left)
    { 
        find = find->left; 
    } 
    return find; 
}

tree* left_follower_key(tree *root) 
{ 
    if(root == NULL) 
        return root; 
    if(root->right) 
        return min_tree(root->right); 
    
    tree *y = root->parent; 
    tree *x = root; 
    while(y && x == y->right)
    { 
        x = y; 
        y = y->parent; 
    } 
    return y; 
}

void delete(tree **root, tree* pt) 
{ 
    if (root == NULL || *root == NULL || pt == NULL)
        return;

    tree *remove = NULL, *remove_son = NULL; 

    if(pt->left == NULL || pt->right == NULL) 
        remove = pt; 
    else 
        remove = left_follower_key(pt);

    if(remove->left != NULL)
        remove_son = remove->left; 
    else 
        remove_son = remove->right; 

    if(remove_son) 
        remove_son->parent = remove->parent; 

    if(remove->parent == NULL)
        *root = remove_son;
    else if (remove == remove->parent->left) 
        remove->parent->left = remove_son; 
    else 
        remove->parent->right = remove_son; 

    if(pt != remove) 
        pt->key = remove->key; 

    free(remove); 
}

// Функция полного освобождения памяти дерева
void free_tree(tree *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(void)
{
    int num;
    char ch = ' ';
    tree *root = NULL;

    while (num && scanf("%d%c", &num, &ch) >= 1)
    {
        if (!num) break;
        insert(&root, num, NULL);
    }

    btUpView(root);
    free_tree(root);
    
    return 0;
}
