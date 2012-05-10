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
	printf("| Program A Start |\n");	
	printf("+-----------------+\n");
	result = seminit(2000, 1);
	printf("<1> Create semaphore named 2000, with value 1: result=%d & e=%d\n", result, e);
	
	result = semvalue(2000);
	printf("<2> Look at value of semaphore[index for indentifier(2000)]: result=%d & e=%d\n", result, e);
	
	printf("<3> I am to do nothing for 10 seconds.\n");
	int wait_unit;
	for (wait_unit = 1; wait_unit < 11; wait_unit++) {
		sleep(1);
		printf("   Waiting %d\n", wait_unit);
	}
	printf("<4> I done doing nothing.\n");

	result = semup(2000);
	printf("<5> Sem Up: result=%d & e=%d\n", result, e);
	
	printf("<6> Program A has no more to do.\n", result, e);
	
	return 0;
}
