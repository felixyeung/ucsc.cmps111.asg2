#include <stdio.h>
#include <stdlib.h>
#include <sema.h>
#include <errno.h>

int main() {
	int result;
	int e;
	message m;
	e = errno;
	
	result = seminit(2000, -1);
	printf("Create semaphore: result=%d & e=%d\n", result, e);
	
	result = semvalue(2000);
	printf("semaphore value of the thing we just created: result=%d & e=%d\n", result, e);
	
	result = semfree(2000);
	printf("Free semaphore (return 1 if successful): result=%d & e=%d\n", result, e);
	
	result = semvalue(2000);
	printf("Can we still see value? this should fail: result=%d & e=%d\n", result, e);
	
	return 0;
}
