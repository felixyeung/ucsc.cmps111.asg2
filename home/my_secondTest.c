#include <stdio.h>
#include <stdlib.h>
#include <sema.h>
#include <errno.h>

int main() {
	int i;
	int e;
	message m;
	e = errno;
	seminit(2000, -100);
	i = semvalue(2000);
	printf("Test result: i=%d & e=%d\n", i, e);
	return 0;
}
