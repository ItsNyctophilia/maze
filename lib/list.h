#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct list_ list;

list *list_create(void (*item_destroy)(void *));

void list_destroy(list *l);

size_t list_size(const list *l);

// Returns the void * at position idx.
// If idx is out of bounds or l is NULL, returns NULL
void *list_get(const list *l, size_t idx);

void list_set(list *l, size_t idx, void *new_val);

void list_append(list *l, void *new_val);

void list_prepend(list *l, void *new_val);

// Calls func() on each element in the list l
void list_iterate(list *l, void (*func)(void *));

#endif
