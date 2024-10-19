#include <utils/linked_list.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct linked_list *linked_list_new()
{
    struct linked_list *list = malloc(sizeof(struct linked_list));
    *list = (struct linked_list){NULL, NULL, 0};
    return list;
}

void linked_list_free(struct linked_list *list)
{
    struct node *curr = list->head;
    while (curr != NULL)
    {
        struct node *next = curr->next;
        free(curr);
        curr = next;
    }
    free(list);
}

void linked_list_append(struct linked_list *list, void *data)
{
    struct node *new_node = malloc(sizeof(struct node));
    *new_node = (struct node){data, list->tail, NULL};
    if (list->tail != NULL)
    {
        list->tail->next = new_node;
    }
    list->tail = new_node;
    if (list->head == NULL)
    {
        list->head = new_node;
    }
    list->size++;
}

void *linked_list_pop(struct linked_list *list)
{
    if (list->size == 0)
    {
        return NULL;
    }
    struct node *node = list->tail;
    void *data = node->data;
    list->tail = node->prev;
    if (list->tail != NULL)
    {
        list->tail->next = NULL;
    }
    else
    {
        list->head = NULL;
    }
    free(node);
    list->size--;
    return data;
}

void linked_list_print(struct linked_list *list, void (*print)(void *))
{
    struct node *node = list->head;
    while (node != NULL)
    {
        print(node->data);
        node = node->next;
    }
}

size_t linked_list_size(const struct linked_list *list)
{
    return list->size;
}