#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/graph.h"
#include "lib/path.h"

enum {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2
};

union int_as_void {
	int num;
	void *ptr;
};

int maze_node_cmp(const void *src, const void *dst);
void dimensions_of_maze(FILE * fo, int *height, int *width);
double find_weight(char target);
graph *load_maze(FILE * fo, char **maze, int height, int width);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s mazefile\n", argv[0]);
		return INVOCATION_ERROR;
	}

	FILE *fo = fopen(argv[1], "r");
	if (!fo) {
		perror("Could not open maze file");
		return FILE_ERROR;
	}
	char *maze;
	int height;
	int width;
	dimensions_of_maze(fo, &height, &width);
	graph *g = load_maze(fo, &maze, height, width);

	for (int i = 0; i < height; ++i) {
		printf("%.*s\n", (int)width, maze + (width * i));
	}
	graph_destroy(g);

	free(maze);
	fclose(fo);
	return SUCCESS;
}

int maze_node_cmp(const void *src, const void *dst)
{
	return (union int_as_void *)src - (union int_as_void *)dst;
}

graph *load_maze(FILE * fo, char **maze, int height, int width)
{
	char *line_buf = NULL;
	size_t buf_size = 0;
	*maze = malloc(((height * width) + 1) * sizeof(**maze));
	if (!*maze) {
		fprintf(stderr, "Memory allocation error");
	}

	memset(*maze, ' ', width * height);	// Set all bytes except last to ' '
	memset(*maze + (width * height), '\0', 1);	// Null terminator
	memset(*maze, 'X', width);	// Top row
	memset(*maze + ((height * width) - width), 'X', width);	// Bottom row
	size_t counter = 1;	// Offset from top row
	while (getline(&line_buf, &buf_size, fo) != -1) {
		if (strchr(line_buf, '\n')) {
			char *tmp = strchr(line_buf, '\n');
			*tmp = ' ';
		}
		memset(*maze + (width * counter), 'X', 1);
		memcpy(*maze + (width * counter + 1), line_buf,
		       strlen(line_buf));
		memset(*maze + (width * counter + (width - 1)), 'X', 1);
		++counter;
	}

	graph *g = graph_create(maze_node_cmp, NULL);
	for (int i = 0; i < height * width - 1; ++i) {
		if ((*maze + i)[0] != '#') {
			// Case: node is not a wall
			union int_as_void num = {.num = i };
			graph_add_node(g, num.ptr);
		}
		if (i % width) {
			// Case: node has left neighbor
			union int_as_void curr = {.num = i };
			union int_as_void prev = {.num = i - 1 };
			if ((*maze + i - 1)[0] == '#') {
				// Case: left neighbor is wall
				continue;
			}
			graph_add_edge(g, prev.ptr, curr.ptr,
				       find_weight((*maze + i)[0]));
			graph_add_edge(g, curr.ptr, prev.ptr,
				       find_weight((*maze + i - 1)[0]));
		}
		if ((i - width) > -1) {
			// Case: node has an above neighbor
			union int_as_void curr = {.num = i };
			union int_as_void up = {.num = i - width };
			if ((*maze + i - width)[0] == '#') {
				// Case: above neighbor is wall
				continue;
			}
			graph_add_edge(g, up.ptr, curr.ptr,
				       find_weight((*maze + i)[0]));
			graph_add_edge(g, curr.ptr, up.ptr,
				       find_weight((*maze + i - width)[0]));
		}
	}

	if (line_buf) {
		free(line_buf);
	}

	return (g);
}

double find_weight(char target)
{
	switch (target) {
	case '@':
	case '>':
	case ' ':
		return 1;
	case 'X':
		return .5;
	case '#':
		return 11;
	}
	return 0;
}

void dimensions_of_maze(FILE * fo, int *height, int *width)
{
	*height = 2;
	*width = 2;
	char *line_buf = NULL;
	size_t buf_size = 0;
	while (getline(&line_buf, &buf_size, fo) != -1) {
		strtok(line_buf, "\n");
		if (strlen(line_buf) > *width - 2) {
			*width = strlen(line_buf) + 2;
		}
		++*height;
	}
	if (line_buf) {
		free(line_buf);
	}
	fseek(fo, 0, SEEK_SET);
}
