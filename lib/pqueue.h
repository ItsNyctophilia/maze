#ifndef PQUEUE_H
#define PQUEUE_H

#include <stdbool.h>

typedef struct pq_ pqueue;

enum pqueue_type { MIN_PQUEUE, MAX_PQUEUE };

pqueue *pqueue_create(enum pqueue_type type);

bool pqueue_is_empty(const pqueue *pq);

//TODO
bool pqueue_contains(const pqueue *pq, const void *data);

//TODO
double pqueue_get_priority(const pqueue *pq, const void *data);

bool pqueue_enqueue(pqueue *pq, double priority, void *data);

void *pqueue_dequeue(pqueue *pq, double *priority);

void pqueue_destroy(pqueue *pq);



#endif
