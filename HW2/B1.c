/*
Описана структура данных:
typedef struct list {
    void *address;
    size_t size;
    char comment[64];
    struct list *next;
} list;
Требуется реализовать только одну функцию, которая 
анализирует данный список и возвращает сколько всего 
памяти используется. Адрес хранится в поле address, 
поле size - соответствующий размер данного блока. 
Если список пустой, то функция должна возвращать 0.
Прототип функции: size_t totalMemoryUsage(list *head) 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SIZE_S 64

typedef struct list
{
    void *address;
    size_t size;
    char comment[64];
    struct list *next;
} list;

list* list_create (void *address, size_t size, const char* comment)
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

void list_push_front (list** head, void *address, size_t size, const char* comment)
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

size_t totalMemoryUsage(list *head)
{
    if (!head)
    {
        return 0;
    }
    
    list *tmp_head = head;
    size_t totalsize = 0;
    
    while (tmp_head != NULL)
    {
        totalsize += tmp_head->size;
        tmp_head = tmp_head->next;
    }
    return totalsize;
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
            list_push_front(&head, (void*)(uintptr_t)addr, size, "");
        }
        else
        {
            goto exit;
        }
    }

    printf("%zu\n", totalMemoryUsage(head));

    exit:
    list_free(&head);

    return EXIT_SUCCESS;
}