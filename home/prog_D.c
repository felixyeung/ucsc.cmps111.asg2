#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sema.h>
#include <errno.h>

/* what does this program do?
   First we make a semaphore
   check what its value is
   do semdown on it
   start sleep 10 seconds
	-> at this point, another program would try to semdown the semaphore we created
   end sleep
   now do a semup, program B should be able to proceed.
   
	-> program C should now free the semaphore, and try a semvalue on it.
   */
int main() {
	int result;
	int e;
	message m;
	e = errno;
	
	printf("+-----------------+\n");
	printf("| Program D Start |\n");	
	printf("+-----------------+\n");

	result = semfree(2000);
	printf("<1> Free semaphore (return 1 if successful): result=%d & e=%d\n", result, errno);
	
	result = semvalue(2000);
	printf("<2> Can we still see value? this should fail: result=%d & e=%d\n", result, errno);
	
	return 0;
}
