#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/graph.h"
#include "lib/path.h"

enum {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	INVALID_MAP = 3
};

union int_as_void {
	int num;		// TODO: Change this to long
	void *ptr;
};

char *maze;

int maze_node_cmp(const void *src, const void *dst);
void dimensions_of_maze(FILE * fo, int *height, int *width);
double find_weight(char target);
graph *load_maze(FILE * fo, char **maze, int height, int width);

void add_path(void *data)
{
	if (maze[(long)data] != '@' && maze[(long)data] != '>') {
		maze[(long)data] = '.';
	}
	return;
}

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
	int height;
	int width;
	dimensions_of_maze(fo, &height, &width);
	graph *g = load_maze(fo, &maze, height, width);
	// TODO: Validate symbols in maze
	union int_as_void start = {.num = (strchr(maze, '@')) - maze };
	union int_as_void finish = {.num = (strchr(maze, '>')) - maze };
	union int_as_void test_finish = {.num = 1 };
	list *test_path = dijkstra_path(g, start.ptr, test_finish.ptr);
	if (list_size(test_path)) {
		fprintf(stderr, "Error: unbounded maze\n");
		graph_destroy(g);
		list_destroy(test_path);
		free(maze);
		fclose(fo);
		return (INVALID_MAP);
	}
	list *path = dijkstra_path(g, start.ptr, finish.ptr);
	list_iterate(path, add_path);
	//Print array
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			if (maze[j + (width * i)] != 'X') {
				printf("%.1s", maze + (j + (width * i)));
			}
		}
		if (i != 0 && i != height - 1) {
			// Avoid unnecessary newlines for start and end of maze
			putchar('\n');
		}

	}

	graph_destroy(g);
	list_destroy(test_path);
	list_destroy(path);
	free(maze);
	fclose(fo);
	return SUCCESS;
}

int maze_node_cmp(const void *src, const void *dst)
{
	return (long)src - (long)dst;
}

graph *load_maze(FILE * fo, char **maze, int height, int width)
{
	char *line_buf = NULL;
	size_t buf_size = 0;
	*maze = malloc(((height * width)) * sizeof(**maze));
	if (!*maze) {
		fprintf(stderr, "Memory allocation error");
	}

	memset(*maze, ' ', width * height);	// Set all bytes except last to ' '
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
		// TODO: fix index 0
		if ((*maze + i)[0] == '#') {
			continue;
		}
		union int_as_void num = {.num = i };
		graph_add_node(g, num.ptr);
		if (i % width != 0) {
			// Case: node has left neighbor
			union int_as_void curr = {.num = i };
			union int_as_void prev = {.num = i - 1 };
			graph_add_edge(g, prev.ptr, curr.ptr,
				       find_weight((*maze + i)[0]));
			graph_add_edge(g, curr.ptr, prev.ptr,
				       find_weight((*maze + i - 1)[0]));
		}
		if ((i - width) >= 0) {
			// Case: node has an above neighbor
			union int_as_void curr = {.num = i };
			union int_as_void up = {.num = i - width };
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
		return 9999;
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
		// Width will never be negative, this is a safe case
		if (strlen(line_buf) > (unsigned)*width - 2) {
			*width = strlen(line_buf) + 2;
		}
		++*height;
	}
	if (line_buf) {
		free(line_buf);
	}
	fseek(fo, 0, SEEK_SET);
}
