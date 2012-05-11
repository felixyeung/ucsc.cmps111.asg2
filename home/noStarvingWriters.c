#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lockcond.h>

lock mutex;
int writes;
int numWriters=0, numReaders=0;


void writer() {
	int p = getpid();
	printf("%d: writer initialized\n", p);
	lock_acquire(&mutex);
	printf("%d: writer has acquired mutex\n", p);
	numWriters += 1;
	semdown(writes);
	printf("%d: writer has grabbed the semaphore\n", p);
	lock_release(&mutex);
	printf("%d: writer has released mutex; now writing\n", p);
	usleep(rand() % 20000 + 200);
	printf("%d: writer is done writing\n", p);
	lock_acquire(&mutex);
	printf("%d: writer has acquired mutex\n", p);
	semup(writes);
	numWriters -= 1;
	printf("%d: writer has released the semaphore\n", p);
	lock_release(&mutex);
	printf("%d: writer has released mutex\n", p);
	printf("%d: writer terminated\n", p);
}

void reader() {
	int p = getpid();
	printf("%d: reader initialized\n", p);
	lock_acquire(&mutex);
	printf("%d: reader has acquired mutex\n", p);
	if(numReaders == 0 || numWriters > 0) {
		semdown(writes);
		printf("%d: reader has grabbed the semaphore\n", p);
	}
	numReaders += 1;
	lock_release(&mutex);
	printf("%d: reader has released mutex; now reading\n", p);
	usleep(rand() % 20000 + 200);
	printf("%d: reader is done reading\n", p);
	lock_acquire(&mutex);
	printf("%d: reader has acquired mutex\n", p);
	numReaders -= 1;
	if(numReaders == 0) {
		semup(writes);
		printf("%d: reader has released the semaphore\n", p);
	}
	lock_release(&mutex);
	printf("%d: reader has released mutex\n", p);
	printf("%d: reader terminated\n", p);
}

int main() {
	printf("BEGIN NO STARVING WRITER\n");
	pid_t pid;
	int i, numProcs = 20;

	lock_init(&mutex);
	writes = seminit(0, 1);
	
	srand(time(NULL));

	for(i=0; i<numProcs; i++) {
		
		pid = fork();

		if( pid == 0 ) {
			printf("new child is...");
			if(rand()%1 == 0) {
				printf("reader!\n");
				reader();
			} else {
				printf("writer!\n");
				writer();
			}
		}
	}
	lock_release(&mutex);
	semfree(writes);
	printf("END NO STARVING WRITERS\n");
}
