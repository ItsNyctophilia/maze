#include "map.h"

#include <math.h>
#include <string.h>
#include <openssl/md5.h>

struct node {
	const char *key;
	void *value;
	struct node *next;
};

struct map_ {
	struct node **data;
	size_t size;
	size_t capacity;
};

// Seems like a reasonable starting size?
// Seems like a reasonable load factor for a hashtable (out of 100)
enum { STARTING_HASHTABLE_SIZE = 15, LOAD_FACTOR = 70 };

static size_t hash(const char *key, size_t capacity);

map *map_create(void)
{
	map *m = malloc(sizeof(*m));
	if (!m) {
		return NULL;
	}

	m->size = 0;
	m->capacity = STARTING_HASHTABLE_SIZE;

	m->data = calloc(STARTING_HASHTABLE_SIZE, sizeof(*m->data));
	if (!m->data) {
		free(m);
		return NULL;
	}

	return m;
}

size_t map_size(const map *m)
{
	if (!m) {
		return 0;
	}

	return m->size;
}

bool map_set(map *m, const char *key, void *value)
{
	if (!m || !key) {
		return false;
	}

	if (100 * m->size / m->capacity > LOAD_FACTOR) {
		struct node **copy = calloc(2 * m->capacity, sizeof(*copy));
		if (!copy) {
			return false;
		}

		for (size_t n=0; n < m->capacity; ++n) {
			struct node *curr = m->data[n];
			while (curr) {
				struct node *tmp = curr->next;

				// Doubled because the copy has twice as much capacity
				size_t idx = hash(curr->key, 2 * m->capacity);

				curr->next = copy[idx];
				copy[idx] = curr;
				curr = tmp;
			}
		}

		m->capacity *= 2;

		free(m->data);
		m->data = copy;
	}

	size_t idx = hash(key, m->capacity);

	struct node *curr = m->data[idx];
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			curr->value = value;
			return true;
		}
		curr = curr->next;
	}

	struct node *new = malloc(sizeof(*new));
	if (!new) {
		return false;
	}

	new->key = strdup(key);
	new->value = value;
	new->next = m->data[idx];

	m->data[idx] = new;
	m->size++;

	return true;
}

void *map_get(const map *m, const char *key)
{
	if (!m || !key) {
		return NULL;
	}

	size_t idx = hash(key, m->capacity);

	struct node *curr = m->data[idx];
	while (curr) {
		if (strcmp(curr->key, key) == 0) {
			return curr->value;
		}

		curr = curr->next;
	}

	return NULL;
}

void map_iterate(const map *m, void (*func)(const char *, void *))
{
	if (!m || !func) {
		return;
	}

	for (size_t n=0; n < m->capacity; ++n) {
		struct node *curr = m->data[n];
		while (curr) {
			func(curr->key, curr->value);
			curr = curr->next;
		}
	}

}

void map_destroy(map *m)
{
	if (!m) {
		return;
	}

	for (size_t n=0; n < m->capacity; ++n) {
		struct node *curr = m->data[n];
		while (curr) {
			// map owns the storage for each key via strdup
			// Safe to cast away const-ness
			free((char *)curr->key);
			struct node *to_free = curr;
			curr = curr->next;
			free(to_free);
		}
	}

	free(m->data);
	free(m);
}

static size_t hash(const char *key, size_t capacity)
{
	union {
		unsigned char buffer[MD5_DIGEST_LENGTH];
		size_t index;
	} result;

	// MD5 takes array of bytes, and a string is an array of bytes
	MD5((const unsigned char *)key, strlen(key), result.buffer);

	return result.index % capacity;
}
