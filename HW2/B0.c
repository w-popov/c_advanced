/*
    Описана структура данных
    typedef struct list {
        uint64_t address;
        size_t size;
        char comment[64];
        struct list *next;
    } list;
    Требуется реализовать только одну функцию, которая в данном списке 
    находит адрес блока памяти занимающий больше всего места.
    Адрес хранится в поле address, поле size - соответствующий размер 
    данного блока. Если список пустой, то функция должна возвращать 0. 
    Если есть несколько таких блоков, то вернуть адрес любого из них.
    Прототип функции: uint64_t findMaxBlock(list *head)
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define SIZE_S 64

typedef struct list
{
    uint64_t address;
    size_t size;
    char comment[SIZE_S];
    struct list *next;
} list;

list* list_create (uint64_t address, size_t size, const char* comment)
{
    list* new_node = malloc(sizeof(list));
    if (!new_node) 
    {
        perror("Allocation failed\n");
        return NULL;
    }

    new_node->address = address;
    new_node->size = size;
    new_node->next = NULL;

    if (comment)
    {
        snprintf(new_node->comment, SIZE_S, "%s", comment);
    } 
    else 
    {
        new_node->comment[0] = '\0';
    }
    
    return new_node;
}

void list_push_front (list** head, uint64_t address, size_t size, const char* comment)
{
    list* new_node = list_create(address, size, comment);
    if (!new_node) 
        return;
    
    new_node->next = *head;
    *head = new_node;
}

void list_free (list** head)
{
    list* current = *head;
    while (current != NULL)
    {
        list* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}

uint64_t findMaxBlock(list *head)
{
    if (!head)
    {
        return 0;
    }
    
    list *tmp_head = head;
    uint64_t maxblock = tmp_head->address;
    size_t maxsize    = tmp_head->size;
    
    while (tmp_head != NULL)
    {
        if (tmp_head->size > maxsize)
        {
            maxsize = tmp_head->size;
            maxblock = tmp_head->address;
        }
        tmp_head = tmp_head->next;
    }
    return maxblock;
}

int main (void)
{
    list* head = NULL;
    int nums = 0;
    uint64_t addr = 0;
    size_t size = 0;

    if (scanf("%d", &nums) != 1)
    { 
        goto exit;
    }
    for (int i = 0; i < nums; ++i)
    {
        if (scanf("%lX %zu", &addr, &size) == 2)
        {
            list_push_front(&head, addr, size, "");
        }
        else
        {
            goto exit;
        }
    }

    printf("%lX\n", findMaxBlock(head));

    exit:
    list_free(&head);

    return EXIT_SUCCESS;
}
