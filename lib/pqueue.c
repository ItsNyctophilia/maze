#include "pqueue.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

struct node {
	double priority;
	void *value;
};

struct pq_ {
	struct node **data;
	size_t size;
	size_t capacity;
	// Is left more urgent than right
	bool (*is_more_urgent_than)(double left, double right);
};

static bool min_heap_cmp(double left, double right)
{
	//TODO: Process NANs as less urgent
	return left < right;
}

static bool max_heap_cmp(double left, double right)
{
	//TODO: Process NANs as less urgent
	return left > right;
}


// Seems like a good pick for the default?
enum { DEFAULT_CAPACITY=16 };

pqueue *pqueue_create(enum pqueue_type type)
{
	pqueue *pq = malloc(sizeof(*pq));
	if (!pq) {
		return NULL;
	}

	pq->size = 0;
	pq->capacity = DEFAULT_CAPACITY;
	if (type == MIN_PQUEUE) {
		pq->is_more_urgent_than = min_heap_cmp;
	} else {
		pq->is_more_urgent_than = max_heap_cmp;
	}

	pq->data = malloc(pq->capacity * sizeof(*pq->data));
	if (!pq->data) {
		free(pq);
		return NULL;
	}

	return pq;
}

bool pqueue_is_empty(const pqueue *pq)
{
	if (!pq) {
		return false;
	}

	return pq->size == 0;
}

static size_t parent(size_t idx)
{
	return (idx - 1) / 2;
}

static size_t left(size_t idx)
{
	return 2*idx + 1;
}

static size_t right(size_t idx)
{
	return 2*idx + 2;
}

static void bubble_up(pqueue *pq)
{
	size_t idx = pq->size - 1;

	while (idx > 0) {
		size_t parent_idx = parent(idx);

		if (pq->is_more_urgent_than(pq->data[idx]->priority,
					pq->data[parent_idx]->priority)) {
			// Swap
			struct node *tmp = pq->data[idx];
			pq->data[idx] = pq->data[parent_idx];
			pq->data[parent_idx] = tmp;
		} else {
			return;
		}

		idx = parent_idx;
	}
}

bool pqueue_enqueue(pqueue *pq, double priority, void *data)
{
	if (!pq || !data) {
		return false;
	}

	struct node *new = malloc(sizeof(*new));
	if (!new) {
		return false;
	}

	new->priority = priority;
	new->value = data;

	// TODO resize if size == capacity

	pq->data[pq->size] = new;
	pq->size++;

	bubble_up(pq);

	return true;
}

static void bubble_down(pqueue *pq)
{
	size_t idx = 0;
	while (idx < pq->size) {
		size_t left_idx = left(idx);
		size_t right_idx = right(idx);

		// TODO refactor to simplify
		if (left_idx >= pq->size) {
			return;
		} else if (right_idx >= pq->size) {
			if (pq->is_more_urgent_than(pq->data[left_idx]->priority,
						pq->data[idx]->priority)) {
				struct node *tmp = pq->data[idx];
				pq->data[idx] = pq->data[left_idx];
				pq->data[left_idx] = tmp;
				return;
			}
		}

		size_t to_swap_idx = left_idx;
		if (pq->is_more_urgent_than(pq->data[right_idx]->priority,
					pq->data[left_idx]->priority)) {
			to_swap_idx = right_idx;
		}

		if (pq->is_more_urgent_than(pq->data[idx]->priority,
					pq->data[to_swap_idx]->priority)) {
			return;
		}
		struct node *tmp = pq->data[idx];
		pq->data[idx] = pq->data[to_swap_idx];
		pq->data[to_swap_idx] = tmp;

		idx = to_swap_idx;
	}
}

void *pqueue_dequeue(pqueue *pq, double *priority)
{
	if (pqueue_is_empty(pq)) {
		return NULL;
	}

	void *result = pq->data[0]->value;

	if (priority) {
		*priority = pq->data[0]->priority;
	}

	free(pq->data[0]);
	pq->data[0] = pq->data[pq->size - 1];
	pq->size--;

	bubble_down(pq);

	return result;
}

bool pqueue_contains(const pqueue *pq, const void *data)
{
	if (!pq || !data) {
		return false;
	}

	for (size_t n=0; n < pq->size; ++n) {
		//TODO Does this need to be swapped with a comparison function?
		if (pq->data[n]->value == data) {
			return true;
		}
	}

	return false;
}

double pqueue_get_priority(const pqueue *pq, const void *data)
{
	if (!pq || !data) {
		return NAN;
	}

	for (size_t n=0; n < pq->size; ++n) {
		//TODO Does this need to be swapped with a comparison function?
		if (pq->data[n]->value == data) {
			return pq->data[n]->priority;
		}
	}

	return NAN;
}

void pqueue_destroy(pqueue *pq)
{
	if (!pq) {
		return;
	}

	for (size_t n=0; n < pq->size; ++n) {
		free(pq->data[n]);
	}
	free(pq->data);

	free(pq);
}
