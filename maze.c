#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2
};

void dimensions_of_maze(FILE * fo, size_t *height, size_t *width);
void load_maze(FILE * fo, char **maze, size_t height, size_t width);

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s mazefile\n", argv[0]);
		return (INVOCATION_ERROR);
	}

	FILE *fo = fopen(argv[1], "r");
	if (!fo) {
		perror("Could not open maze file");
		return FILE_ERROR;
	}
	char *maze;
	size_t height;
	size_t width;
	dimensions_of_maze(fo, &height, &width);
	load_maze(fo, &maze, height, width);

	for (size_t i = 0; i < height; ++i) {
		printf("%.*s\n", (int)width, maze + (width * i));
	}
	free(maze);
	fclose(fo);
	return (SUCCESS);
}

void load_maze(FILE * fo, char **maze, size_t height, size_t width)
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

	if (line_buf) {
		free(line_buf);
	}

	return;
}

void dimensions_of_maze(FILE * fo, size_t *height, size_t *width)
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
