#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>

struct node {
    void* data;
    struct node* prev;
    struct node* next;
};

struct linked_list {
    struct node* head;
    struct node* tail;
    size_t size;
};

struct linked_list* linked_list_new();

void linked_list_free(struct linked_list* list);

void linked_list_append(struct linked_list* list, void* data);

void linked_list_prepend(struct linked_list* list, void* data);

void* linked_list_pop(struct linked_list* list);

void* linked_list_remove(struct linked_list* list, size_t index);

void* linked_list_get(struct linked_list* list, size_t index);

void linked_list_insert(struct linked_list* list, size_t index, void* data);

void linked_list_set(struct linked_list* list, size_t index, void* data);

void linked_list_print(struct linked_list* list, void (*print)(void*));

size_t linked_list_size(const struct linked_list* list);

#endif // LINKED_LIST_H