#ifndef PATH_H
#define PATH_H

#include "list.h"
#include "graph.h"

list *dijkstra_path(const graph *g, const void *start, const void *end);

#endif
