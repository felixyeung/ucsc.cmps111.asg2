#include <stdio.h>
#include <stdlib.h>
#include <sema.h>
#include <errno.h>

int main() {
	int i;
	int e;
	message m;
	e = errno;
	i = seminit(0, 0);
	printf("Test result: i=%d & e=%d\n", i, e);
	return 0;
}
