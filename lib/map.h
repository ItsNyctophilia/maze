#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct map_ map;


map *map_create(void);

size_t map_size(const map *m);

bool map_set(map *m, const char *key, void *value);

void *map_get(const map *m, const char *key);

void map_iterate(const map *m, void (*func)(const char *, void *));

void map_destroy(map *m);


#endif
