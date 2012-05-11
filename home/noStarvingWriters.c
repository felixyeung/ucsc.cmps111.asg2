#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lockcond.h>
#include <sema.h>

lock mutex;
int write;
int numWriters=0, numReaders=0;


void writer() {
	int p = getpid();
	printf("%d: writer initialized\n", p);
	lock_acquire(mutex);
	printf("%d: writer has acquired mutex\n", p);
	numWriters += 1;
	semdown(write);
	printf("%d: writer has grabbed the semaphore\n", p);
	lock_release(mutex);
	printf("%d: writer has released mutex; now writing\n", p);
	sleep(rand() % 200 + 20);
	printf("%d: writer is done writing\n", p);
	lock_acquire(mutex);
	printf("%d: writer has acquired mutex\n", p);
	semup(write);
	numWriters -= 1;
	printf("%d: writer has released the semaphore\n", p);
	lock_release(mutex);
	printf("%d: writer has released mutex\n", p);
	printf("%d: writer terminated\n", p);
}

void reader() {
	int p = getpid();
	printf("%d: reader initialized\n", p);
	lock_acquire(mutex);
	printf("%d: reader has acquired mutex\n", p);
	if(numReaders == 0 || numWriteres > 0) {
		semdown(write);
		printf("%d: reader has grabbed the semaphore\n", p);
	}
	numReaders += 1;
	lock_release(mutex);
	printf("%d: reader has released mutex; now reading\n", p);
	sleep(rand() % 200 + 20);
	printf("%d: reader is done reading\n", p);
	lock_acquire(mutex);
	printf("%d: reader has acquired mutex\n", p);
	numReaders -= 1;
	if(numReaders == 0) {
		semup(write);
		printf("%d: reader has released the semaphore\n", p);
	}
	lock_release(mutex);
	printf("%d: reader has released mutex\n", p);
	printf("%d: reader terminated\n", p);
}

int main() {
	printf("BEGIN NO STARVING WRITER\n");
	pid_t pid;
	int i, numProcs = 20;

	lock_init(mutex);
	write = seminit(0, 1);
	
	srand(time(NULL));

	for(i=0; i<numProcs; i++) {
		pid = fork();

		if( pid == -1 ) {
			printf("ERROR ON FORK!");
			return -1;
		}

		if( pid == 0 ) {
			if(rand()%1 == 0) {
				reader();
			} else {
				writer();
			}
		}
	}
	printf("END NO STARVING WRITERS\n");
}
