#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lib/graph.h"		// libraries and dependencies taken from Liam Echlin
#include "lib/path.h"

enum {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2,
	MEMORY_ERROR = 3,
	INVALID_MAP = 4
};

union int_as_void {
	long num;		// TODO: Change this to long
	void *ptr;
};

static struct {
	bool doors;
	bool water;
} options = { false, false };

char *maze;			// global so that add_path can modify 

int maze_node_cmp(const void *src, const void *dst);
void dimensions_of_maze(FILE * fo, int *height, int *width);
double find_weight(char target);
graph *load_maze(FILE * fo, char **maze, int height, int width);
void add_path(void *data);

int main(int argc, char *argv[])
{
	int opt;
	while ((opt = getopt(argc, argv, "dw")) != -1) {
		switch (opt) {
		case 'd':
			options.doors = true;
			break;
		case 'w':
			options.water = true;
			break;
		case '?':
			return (INVOCATION_ERROR);
		}
	}

	argc -= optind;
	argv += optind;

	if (argc != 1) {
		fprintf(stderr, "Usage: ./maze mazefile\n");
		return INVOCATION_ERROR;
	}

	FILE *fo = fopen(argv[0], "r");
	if (!fo) {
		perror("Could not open maze file");
		return FILE_ERROR;
	}
	int height;
	int width;
	dimensions_of_maze(fo, &height, &width);
	graph *g = load_maze(fo, &maze, height, width);
	// Add null terminator before using string functions on maze
	memset(maze + height * width, '\0', 1);
	char valid_set[10];	// Enough space to fit all valid chars
	snprintf(valid_set, 10, " #@>X%s%s", options.doors ? "/+" : "",
		 options.water ? "~" : "");
	if (strlen(maze) != strspn(maze, valid_set)) {
		// Case: found disallowed symbols in maze
		fprintf(stderr, "Error: invalid symbol(s) in maze\n");
		graph_destroy(g);
		free(maze);
		fclose(fo);
		return (INVALID_MAP);
	} else if (strlen(maze) == 4) {
		// Case: file was empty (2x2 of 'X' is created by default)
		fprintf(stderr, "Error: empty file\n");
		graph_destroy(g);
		free(maze);
		fclose(fo);
		return (INVALID_MAP);
	}
	union int_as_void start = {.num = (strchr(maze, '@')) - maze };
	union int_as_void finish = {.num = (strchr(maze, '>')) - maze };
	union int_as_void test_finish = {.num = 1 };	// Boundary at index 1
	list *test_path = dijkstra_path(g, start.ptr, test_finish.ptr);
	if (list_size(test_path) != 0 && list_size(test_path) != 1) {
		// Case: maze was not fully bounded, successful path was
		// found to the boundary node at index 1.
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

void add_path(void *data)
{
	if (maze[(long)data] != '@' && maze[(long)data] != '>') {
		maze[(long)data] = '.';
	}
	return;
}

int maze_node_cmp(const void *src, const void *dst)
{
	return (long)src - (long)dst;
}

graph *load_maze(FILE * fo, char **maze, int height, int width)
{
	char *line_buf = NULL;
	size_t buf_size = 0;
	*maze = malloc(((height * width)) * sizeof(**maze) + 1);
	if (!*maze) {
		fprintf(stderr, "Memory allocation error");
		exit(MEMORY_ERROR);
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
		// Known bug: index 0 doesn't get added to the graph because '0'
		// is treated as a false value by graph_add_node. There are no 
		// explicit consequences of this bug, given that it is a boundary node,
		// but it personally annoys me.
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
	case '~':
		return 3;
	case '+':
		return 2;
	case '/':
	case '@':
	case '>':
	case ' ':
		return 1;
	case 'X':
		return .5;
	}
	return 0;
}

void dimensions_of_maze(FILE * fo, int *height, int *width)
{
	// Height and width start at 2, because there is a box of 'X's
	// representing boundary nodes that rings the entire maze.
	*height = 2;
	*width = 2;

	char *line_buf = NULL;
	size_t buf_size = 0;
	while (getline(&line_buf, &buf_size, fo) != -1) {
		strtok(line_buf, "\n");
		// Width will never be negative, this is a safe cast
		if (strlen(line_buf) > (unsigned)*width - 2) {
			*width = strlen(line_buf) + 2;
		}
		++*height;
	}
	if (line_buf) {
		free(line_buf);
	}
	// Reset file pointer position
	fseek(fo, 0, SEEK_SET);
}
