#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	SUCCESS = 0,
	INVOCATION_ERROR = 1,
	FILE_ERROR = 2
};

void load_maze(FILE * fo);

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
	load_maze(fo);
	fclose(fo);
	return (SUCCESS);
}

void load_maze(FILE * fo)
{
	char *line_buf = NULL;
	size_t buf_size = 0;
	size_t height = 2;	// Vertical boundary
	size_t width = 2;	// Horizontal boundary X(...)X
	while (getline(&line_buf, &buf_size, fo) != -1) {
		strtok(line_buf, "\n");
		if (strlen(line_buf) > width - 2) {
			width = strlen(line_buf) + 2;
		}
		++height;
	}
	fseek(fo, 0, SEEK_SET);
	printf("height: %zu, width: %zu\n", height, width);

	if (line_buf) {
		free(line_buf);
	}
	return;
}
