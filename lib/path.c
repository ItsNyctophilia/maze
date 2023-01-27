#include "path.h"

#include "map.h"
#include "pqueue.h"

static const graph *dijkstra_graph;
static pqueue *to_process;
static map *previous;
static map *distance_from_origin;
static const void *curr_item;
static double priority;

union double_pointer {
	double d;
	void *p;
};

static void add_to_pqueue_if_faster(const void *neighbor)
{
	double weight = graph_get_edge_weight(dijkstra_graph, curr_item, neighbor);
	double distance = weight + priority;

	union double_pointer current_best = {.p = map_get(distance_from_origin, neighbor)};

	//TODO refactor out duplication
	if (!map_get(previous, neighbor)) {
		// Nothing in Dijkstra's changes these items or neighbors; this cast is safe
		map_set(previous, neighbor, (void *)curr_item);
		// Nothing in Dijkstra's changes these items or neighbors; this cast is safe
		pqueue_enqueue(to_process, distance, (void *)neighbor);

		current_best.d = distance;
		map_set(distance_from_origin, neighbor, current_best.p);
	} else if (distance < current_best.d) {
		// Nothing in Dijkstra's changes these items or neighbors; this cast is safe
		map_set(previous, neighbor, (void *)curr_item);
		// Nothing in Dijkstra's changes these items or neighbors; this cast is safe
		pqueue_enqueue(to_process, distance, (void *)neighbor);

		current_best.d = distance;
		map_set(distance_from_origin, neighbor, current_best.p);
	}
	
}

list *dijkstra_path(const graph *g, const void *start, const void *end)
{
	// Results are borrowed from the graph g
	list *results = list_create(NULL);

	dijkstra_graph = g;

	to_process = pqueue_create(MIN_PQUEUE);
	previous = map_create();
	distance_from_origin = map_create();

	// Nothing in Dijkstra's changes these items or neighbors; this cast is safe
	pqueue_enqueue(to_process, 0, (void *)start);
	map_set(previous, start, NULL);

	while (!pqueue_is_empty(to_process)) {
		curr_item = pqueue_dequeue(to_process, &priority);

		// TODO
		if (curr_item == end) {
			break;
		}

		graph_iterate_neighbors(g, curr_item, add_to_pqueue_if_faster);
	}

	const void *curr = end;
	// TODO have this be a graph comparison function?
	while (curr) {
		// Nothing in Dijkstra's changes these items or neighbors, but the graph owner
		// may want to, so this cast is safe
		list_prepend(results, (void *)curr);

		curr = map_get(previous, curr);
	}

	map_destroy(distance_from_origin);
	map_destroy(previous);
	pqueue_destroy(to_process);

	return results;
}
