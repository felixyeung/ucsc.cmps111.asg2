#include <stdio.h>
#include <stdlib.h>
#include <sema.h>

int main() {
	int numSems = 10;
	int i;
	for(i=1; i<=numSems; i++) {
		printf("seminit(0, %d)=%d (identifier)\n", i, seminit(0, i));
		sleep(1);
	}
	for(i=1; i<=numSems; i++) {
		printf("semvalue(%d)=%d\n", i, semvalue(i));
		sleep(1);
	}
	for(i=1; i<=numSems; i++) {
		printf("semdown(%d)=%d\n", i, semdown(i));
	}
	for(i=1; i<=numSems; i++) {
		printf("semvalue(%d)=%d\n", i, semvalue(i));
		sleep(1);
	}
	for(i=1; i<=numSems; i++) {
		printf("semfree(%d)=%d\n", i, semfree(i));
	}
	return 0;
}
