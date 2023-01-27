#include "graph.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

struct edge {
	struct node *out;
	double weight;

	// Linked List scaffolding for our edges in a given node
	struct edge *next;
};

struct node {
	void *data;
	struct edge *edges;

	// Linked List scaffolding for our nodes in the graph
	struct node *next;
};

struct graph_ {
	// Head of the linked list of nodes
	struct node *nodes;

	int (*cmp)(const void *a, const void *b);
	void (*destroy)(void *obj);
};


const graph_cmp_func GRAPH_STRCMP = (graph_cmp_func)strcmp;

graph *graph_create(graph_cmp_func cmp, graph_destroy_func destroy)
{
	if (!cmp) {
		return NULL;
	}
	graph *g = malloc(sizeof(*g));
	if (!g) {
		return NULL;
	}

	g->nodes = NULL;
	g->cmp = cmp;
	g->destroy = destroy;

	return g;
}

size_t graph_size(const graph *g)
{
	if (!g) {
		return 0;
	}

	size_t count = 0;
	struct node *curr = g->nodes;
	while (curr) {
		++count;
		curr = curr->next;
	}

	return count;
}

bool graph_add_node(graph *g, void *data)
{
	if (!g || !data) {
		return false;
	}

	// If the data already exist in a node, then bail out
	if (graph_contains(g, data)) {
		return true;
	}

	struct node *new = malloc(sizeof(*new));
	if (!new) {
		return false;
	}

	new->data = data;
	new->edges = NULL;
	new->next = g->nodes;
	g->nodes = new;

	return true;
}

void graph_remove_edge(graph *g, const void *src, const void *dst)
{
	if (!g || !src || !dst) {
		return;
	}

	struct node *from = g->nodes;
	while (from) {
		if (g->cmp(from->data, src) == 0) {
			struct edge **curr = &from->edges;
			while (*curr) {
				if (g->cmp((*curr)->out->data, dst) == 0) {
					struct edge *to_free = *curr;
					*curr = (*curr)->next;
					free(to_free);

					return;
				}

				curr = &(*curr)->next;
			}



		}
		from = from->next;
	}
}

void graph_remove_node(graph *g, void *data)
{
	if (!g || !data) {
		return;
	}

	struct node **curr = &g->nodes;
	while (*curr) {
		if (g->cmp(data, (*curr)->data) == 0) {
			struct node *to_free = *curr;
			*curr = (*curr)->next;

			if (g->destroy) {
				g->destroy(to_free->data);
			}
			struct edge *e = to_free->edges;
			while (e) {
				struct edge *next = e->next;
				free(e);

				e = next;
			}

			free(to_free);
			return;
		}
		curr = &(*curr)->next;
	}
}

bool graph_contains(const graph *g, const void *data)
{
	if (!g || !data) {
		return false;
	}

	struct node *curr = g->nodes;
	while (curr) {
		if (g->cmp(data, curr->data) == 0) {
			return true;
		}

		curr = curr->next;
	}

	return false;
}

bool graph_add_edge(graph *g, void *src, void *dst, double weight)
{
	if (!g || !src || !dst) {
		return false;
	}

	struct node *from = NULL;
	struct node *to = NULL;

	struct node *curr = g->nodes;
	while (curr && (!from || !to)) {
		if (g->cmp(curr->data, src) == 0) {
			from = curr;
		}
		if (g->cmp(curr->data, dst) == 0) {
			to = curr;
		}
		curr = curr->next;
	}
	if (!from || !to) {
		return false;
	}

	struct edge *checker = from->edges;
	while (checker) {
		if (checker->out == to) {
			checker->weight = weight;
			return true;
		}

		checker = checker->next;
	}

	struct edge *new = malloc(sizeof(*new));
	if (!new) {
		return false;
	}

	new->weight = weight;
	new->out = to;
	new->next = from->edges;
	from->edges = new;

	return true;
}

double graph_get_edge_weight(const graph *g, const void *src, const void *dst)
{
	if (!g || !src || !dst) {
		return NAN;
	}

	// This is the node that the edge starts from
	struct node *from = g->nodes;
	while (from) {
		if (g->cmp(from->data, src) == 0) {
			break;
		}
		from = from->next;
	}
	if (!from) {
		return false;
	}

	struct edge *to = from->edges;
	while (to) {
		if (g->cmp(to->out->data, dst) == 0) {
			return to->weight;
		}
		to = to->next;
	}

	return NAN;
}

size_t graph_outdegree_size(const graph *g, const void *from)
{
	if (!g || !from) {
		return 0;
	}

	// This is the node that the edges start from
	struct node *n = g->nodes;
	while (n) {
		if (g->cmp(n->data, from) == 0) {
			break;
		}
		n = n->next;
	}
	if (!n) {
		return 0;
	}

	size_t count = 0;
	struct edge *curr = n->edges;
	while (curr) {
		++count;
		curr = curr->next;
	}

	return count;
}

size_t graph_indegree_size(const graph *g, const void *to)
{
	if (!g || !to) {
		return 0;
	}

	struct node *dest = g->nodes;
	while (dest) {
		if (g->cmp(dest->data, to) == 0) {
			break;
		}
		dest = dest->next;
	}
	if (!dest) {
		return 0;
	}

	size_t count = 0;
	struct node *curr = g->nodes;
	while (curr) {
		struct edge *e = curr->edges;
		while (e) {
			if (e->out == dest) {
				++count;
			}

			e = e->next;
		}

		curr = curr->next;
	}

	return count;
}

void graph_iterate_nodes(const graph *g, void (*func)(const void *))
{
	if (!g || !func) {
		return;
	}

	struct node *curr = g->nodes;
	while (curr) {
		func(curr->data);
		curr = curr->next;
	}
}

void graph_iterate_neighbors(const graph *g, const void *obj,
		void (*func)(const void *))
{
	if (!g || !obj || !func) {
		return;
	}

	struct node *curr = g->nodes;
	while (curr) {
		if (g->cmp(curr->data, obj) == 0) {
			break;
		}

		curr = curr->next;
	}
	if (!curr) {
		return;
	}

	struct edge *e = curr->edges;
	while (e) {
		func(e->out->data);

		e = e->next;
	}
}

void graph_serialize(const graph *g, FILE *output)
{
	if (!g || !output || g->cmp != GRAPH_STRCMP) {
		return;
	}

	struct node *n = g->nodes;
	while (n) {
		printf("%s", (const char *)n->data);

		struct edge *e = n->edges;
		while (e) {
			printf(" %s %f", (const char *)e->out->data, e->weight);
			e = e->next;
		}

		n = n->next;
		putchar('\n');
	}
}

graph *graph_deserialize(FILE *input)
{
	if (!input) {
		return NULL;
	}

	graph *g = graph_create(GRAPH_STRCMP, free);
	if (!g) {
		return NULL;
	}

	char *buffer = NULL;
	size_t buffer_sz = 0;

	while (0 < getline(&buffer, &buffer_sz, input)) {
		char *endline = strchr(buffer, '\n');
		if (endline) {
			*endline = '\0';
		}

		char *word = strtok(buffer, " \t");

		if (!graph_contains(g, word)) {
			char *copy = strdup(word);
			if (!copy) {
				graph_destroy(g);
				return NULL;
			}

			if (!graph_add_node(g, copy)) {
				free(copy);
				graph_destroy(g);
				return NULL;
			}
		}

		while (true) {
			char *dest = strtok(NULL, " \t");
			char *weight_as_string = strtok(NULL, " \t");

			if (!dest || !weight_as_string) {
				break;
			}

			char *err;
			double weight = strtod(weight_as_string, &err);
			if (*err) {
				fprintf(stderr, "Unrecognized weight '%s'; skipping\n",
						weight_as_string);
				continue;
			}

			if (!graph_contains(g, dest)) {
				char *copy = strdup(dest);
				if (!copy) {
					graph_destroy(g);
					return NULL;
				}

				if (!graph_add_node(g, copy)) {
					free(copy);
					graph_destroy(g);
					return NULL;
				}
			}

			if (!graph_add_edge(g, word, dest, weight)) {
				graph_destroy(g);
				return NULL;
			}
		}
	}
	free(buffer);

	return g;
}

void graph_destroy(graph *g)
{
	if (!g) {
		return;
	}

	struct node *curr = g->nodes;
	while (curr) {
		struct edge *e = curr->edges;
		while (e) {
			struct edge *tmp = e->next;
			free(e);
			e = tmp;
		}

		struct node *tmp = curr->next;
		if (g->destroy) {
			g->destroy(curr->data);
		}

		free(curr);

		curr = tmp;
	}

	free(g);
}
