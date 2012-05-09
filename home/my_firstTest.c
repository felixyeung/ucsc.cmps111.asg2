#include <stdio.h>
#include <stdlib.h>
#include <sema.h>
#include <errno.h>

int main() {
	int i = -2, j = -2;
	int e;
	message m;
	m.m1_i1 = 0;
	m.m1_i2 = 0;
	i = _syscall(PM_PROC_NR, 103, &m);
	e = errno;
	j = seminit(0, 0);
	printf("Test results: %d & %d and %d\n", i, e, j);
	return 0;
}
