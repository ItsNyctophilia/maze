#include "list.h"

#include <stdlib.h>

struct list_ {
	struct node *head;
	struct node *last;
	size_t sz;

	void (*item_destroy)(void *);
};

struct node {
	void *data;
	struct node *next;
};

list *list_create(void (*item_destroy)(void *))
{
	list *l = malloc(sizeof(*l));
	if (!l) {
		return NULL;
	}

	l->head = NULL;
	l->last = NULL;
	l->sz = 0;
	l->item_destroy = item_destroy;

	return l;
}

// O(N)
void list_destroy(list *l)
{
	if (!l) {
		return;
	}

	struct node *curr = l->head;
	while (curr) {
		struct node *next = curr->next;
		if (l->item_destroy) {
			l->item_destroy(curr->data);
		}
		free(curr);
		curr = next;
	}

	free(l);
}

// O(1)
size_t list_size(const list *l)
{
	if (!l) {
		return 0;
	}

	return l->sz;
}

// O(N)
void *list_get(const list *l, size_t idx)
{
	if (!l) {
		return NULL;
	}

	struct node *curr = l->head;

	while (curr && idx > 0) {
		curr = curr->next;
		--idx;
	}

	if (!curr) {
		return NULL;
	} else {
		return curr->data;
	}
}

// O(N)
void list_set(list *l, size_t idx, void *new_val)
{
	if (!l) {
		return;
	}

	struct node *curr = l->head;

	while (curr && idx > 0) {
		curr = curr->next;
		--idx;
	}

	if (!curr) {
		return;
	} else {
		curr->data = new_val;
	}
}

// O(1)
void list_append(list *l, void *new_val)
{
	if (!l) {
		return;
	}

	struct node *new = malloc(sizeof(*new));
	if (!new) {
		return;
	}
	new->data = new_val;
	new->next = NULL;

	if (l->last) {
		l->last->next = new;
	} else {
		l->head = new;
	}
	l->last = new;

	l->sz++;
}

// O(1)
void list_prepend(list *l, void *new_val)
{
	if (!l) {
		return;
	}

	struct node *new = malloc(sizeof(*new));
	if (!new) {
		return;
	}
	new->data = new_val;
	new->next = l->head;

	if (!l->last) {
		l->last = new;
	}

	l->sz++;
	l->head = new;
}

// O(N)
// O(1) overhead per item
void list_iterate(list *l, void (*func)(void *))
{
	if (!l || !func) {
		return;
	}

	struct node *curr = l->head;
	while (curr) {
		func(curr->data);
		curr = curr->next;
	}
}
