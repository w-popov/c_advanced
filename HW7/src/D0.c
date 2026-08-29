/**
 * HW7, Cи продвинутый. гр.Д01-134 Попов В.Г
 * D0: Дана строка, состоящая из английских букв и пробелов. 
 * В конце строки символ точка. Все слова разделены одним пробелом. 
 * Необходимо составить из слов односвязный список и упорядочить по алфавиту. 
 * Список необходимо удалить в конце программы. Для сравнение строк можно 
 * использовать strcmp. Необходимо использовать данную структуры организации списка.
    struct list {
    char word[20];
    struct list *next;
    }
 * Необходимо реализовать односвязный список и обслуживающие его функции.
    add_to_list
    swap_elements
    print_list
    delete_list
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define LEN_WORD    20

struct list 
{
    char word[LEN_WORD];
    struct list *next;
};

struct list *add_to_list (struct list **head, const char *word);
void swap_elements (struct list **before_first);
void print_list (struct list *head);
void delete_list (struct list **head);


int main(void)
{
    struct list *head = NULL;
    char word[LEN_WORD] = {'\0'};
    unsigned iter = 1;
    while (iter)
    {
        scanf("%s", word);
        int len = strlen(word);
        
        if (len > 0 && word[len-1] == '.') 
        {
            word[len-1] = '\0';
            iter = 0;
        }
        add_to_list(&head, word);
    }

    print_list(head);
    delete_list(&head);

    return 0;
}


struct list *add_to_list (struct list **head, const char *word)
{
    struct list *tmp = malloc(sizeof(struct list));
    if (tmp == NULL) return NULL;

    strncpy(tmp->word, word, sizeof(tmp->word) - 1);
    tmp->word[sizeof(tmp->word) - 1] = '\0';
    
    tmp->next = *head;
    *head = tmp;

    if ((*head)->next == NULL) 
    {
        return tmp;
    }

    if (strcmp((*head)->word, (*head)->next->word) > 0) 
    {
        swap_elements(head);
    }

    struct list *parent = *head;
    while (parent->next != NULL && parent->next->next != NULL) 
    {
        if (strcmp(parent->next->word, parent->next->next->word) > 0) 
        {
            swap_elements(&(parent->next));
        }
        parent = parent->next;
    }

    return tmp;
}

void print_list (struct list *head)
{
    struct list *tmp = head;
    while (tmp)
    {
        printf("%s ", tmp->word);
        tmp = tmp->next;
    }
    printf("\n");    
}

/* 
Обмен элементов списка включая next-ы 
передачей предыдущего первого из обмениваемых 
*/
void swap_elements (struct list **before_first)
{
    if (!before_first || !*before_first || !(*before_first)->next) 
    {
        return;
    }
    
    struct list *first = *before_first;
    struct list *second = first->next;
    first->next = second->next;
    second->next = first;
    *before_first = second;
}

void delete_list (struct list **head)
{
    if (head == NULL || *head == NULL) 
    {
        return;
    }
    struct list *current = *head;
    struct list *next_node = NULL;

    while (current != NULL) 
    {
        next_node = current->next;
        free(current);
        current = next_node;
    }

    *head = NULL;
}
