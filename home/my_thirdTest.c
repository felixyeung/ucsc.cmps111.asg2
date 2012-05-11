#include <stdio.h>
#include <stdlib.h>
#include <sema.h>
#include <errno.h>

int main() {
	int i, j;
	int e = errno;
	printf("Test results: %d & %d and %d\n", i, e, j);
	return 0;
}
