/**
 * HW7, Cи продвинутый. гр.Д01-134 Попов В.Г
 * D2: В программе описано двоичное дерево:
    typedef struct tree {
        datatype key;
        struct tree *left, *right;
        struct tree *parent; //ссылка на родителя
    } tree;
 * Требуется реализовать функцию, которая по ключу возвращает адрес 
 * соседнего элемента - брата. Если такого ключа нет или у узла нет брата, 
 * то необходимо вернуть 0. Прототип функции:
    tree * findBrother(tree *root, int key)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree 
{
    int key;
    struct tree *left, *right;
    struct tree *parent;
} tree;

void insert(tree **root, int key, tree *pt) 
{ 
    if (!(*root)) 
    {  
        *root = malloc(sizeof(tree)); 
        (*root)->key = key; 
        (*root)->parent = pt;  
        (*root)->left = (*root)->right = NULL;
    } 
    else if (key < (*root)->key) 
        insert(&((*root)->left), key, *root); 
    else 
        insert(&((*root)->right), key, *root); 
}

// поиск узла по ключу
tree* findNode(tree *root, int key) 
{
    if (root == NULL || root->key == key)
        return root;
    
    if (key < root->key)
        return findNode(root->left, key);
    else
        return findNode(root->right, key);
}

tree* findBrother(tree *root, int key) 
{
    tree *node = findNode(root, key);
    
    if (node == NULL || node->parent == NULL) 
        return 0;
    
    tree *p = node->parent;
    
    if (p->left == node) 
        return p->right;
    else 
        return p->left;
}

// Очистка памяти
void freeTree(tree *root) {
    if (!root)
        return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main(void) {
    int num;
    char ch;
    tree *root = NULL;

    while (ch != '\n' && scanf("%d%c", &num, &ch) >= 1) 
    {
        if (num == 0) break; 
        insert(&root, num, NULL);
    }

    int key;
    if (scanf("%d", &key) == 1)
    {
        tree *brother = findBrother(root, key);
        brother ? printf("%d\n", brother->key) : printf("0\n");
    }

    freeTree(root);
    return 0;
}
