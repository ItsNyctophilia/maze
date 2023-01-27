#include <stdio.h>

enum {
	SUCCESS = 0,
	INVOCATION_ERROR = 1
};

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: %s mazefile\n", argv[0]);
		return (INVOCATION_ERROR);
	}
	return (SUCCESS);
}
