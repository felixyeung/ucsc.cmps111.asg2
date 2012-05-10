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
	printf("| Program B Start |\n");	
	printf("+-----------------+\n");
	
	result = semdown(2000);
	printf("<1> Do a sem down: result=%d & e=%d\n", result, errno);
	
	printf("<2> I am to do nothing for 10 seconds.\n");
	int wait_unit;
	for (wait_unit = 1; wait_unit < 11; wait_unit++) {
		sleep(1);
		printf("   Waiting %d\n", wait_unit);
	}
	printf("<3> I done doing nothing.\n");

	result = semup(2000);
	printf("<4> Sem Up: result=%d & e=%d\n", result, errno);
	
	printf("<5> Program B has no more to do.\n", result, errno);
	
	return 0;
}
