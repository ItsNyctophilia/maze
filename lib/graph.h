#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct graph_ graph;

typedef int (*graph_cmp_func)(const void *, const void *);
// Use this as cmp function for a graph to store strings in its nodes
extern const graph_cmp_func GRAPH_STRCMP;

typedef void (*graph_destroy_func)(void *);

// Calls destroy() on each item as they are removed
// (Pass destroy=NULL to not do anything)
graph *graph_create(graph_cmp_func cmp, graph_destroy_func destroy);

// Returns number of nodes in graph
size_t graph_size(const graph *g);

// Adds data to graph g; will return false on invalid graph, data
bool graph_add_node(graph *g, void *data);

void graph_remove_node(graph *g, void *data);

void graph_iterate_nodes(const graph *g, void (*func)(const void *));

void graph_iterate_neighbors(const graph *g, const void *obj,
		void (*func)(const void *));

//TODO Return a pointer to the actual data (essentially graph_get, but then, why?)
bool graph_contains(const graph *g, const void *data);

// Adds edge to graph g between src and dst with weight;
// will return false on invalid graph, src/dst
bool graph_add_edge(graph *g, void *src, void *dst, double weight);

// Returns NAN on invalid graph/src/dst or missing edge
double graph_get_edge_weight(const graph *g, const void *src, const void *dst);

void graph_remove_edge(graph *g, const void *src, const void *dst);

size_t graph_outdegree_size(const graph *g, const void *from);
size_t graph_indegree_size(const graph *g, const void *to);

// These only work for graphs whose data are strings
void graph_serialize(const graph *g, FILE *output);
// Creates a graph that "owns" its strings/data
graph *graph_deserialize(FILE *input);

void graph_destroy(graph *g);

#endif
