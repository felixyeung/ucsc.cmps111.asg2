#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lockcond.h>

lock mutex;
int gender1;
int gender2;
int gender3;
int nurse;
int procreate1;
int procreate2;

void alien(int gender) {
	pid_t p = getpid();
	while(1) {
		lock_acquire(&mutex);
		printf("%d: acquired lock\n", p);
		switch (gender) {
			case 1:
				printf("%d: Gender 1 phase 1\n", p);
				// if there are others waiting
				if (semvalue(gender2) < 0 && semvalue(gender3) < 0) {
					semup(gender2);
					semup(gender3);
					lock_release(&mutex);
					printf("%d: Gender 1 wakes 2, 3. ready to procreate.\n", p);
				}
				else {
					//no other genders queued up
					printf("%d: Cannot pop from 2, 3. 1 going to sleep.\n", p);
					lock_release(&mutex);
					semdown(gender1);
				}
			break;
			case 2:
				printf("%d: Gender 2 phase 1\n", p);
				if (semvalue(gender1) < 0 && semvalue(gender3) < 0) {
					semup(gender1);
					semup(gender3);
					lock_release(&mutex);
					printf("%d: Gender 2 wakes 1, 3. ready to procreate.\n", p);
				}
				else {
					//no other genders queued up
					printf("%d: Cannot pop from 1, 3. 2 going to sleep.\n", p);
					lock_release(&mutex);
					semdown(gender2);
				}
			break;
			case 3:
				printf("%d: Gender 3 phase 1\n", p);
				if (semvalue(gender1) < 0 && semvalue(gender2) < 0) {
					semup(gender1);
					semup(gender2);
					lock_release(&mutex);
					printf("%d: Gender 3 wakes 1, 2. ready to procreate.\n", p);
				}
				else {
					//no other genders queued up
					printf("%d: Cannot pop from 1, 2. 3 going to sleep.\n", p);
					lock_release(&mutex);
					semdown(gender3);
				}
			break;
		}
		switch (gender) {
			case 1:
				printf("%d: Gender 1 phase 2\n", p);
				semdown(procreate1);
				printf("%d: Gender 1 report procreating finished. \n", p);
				usleep(rand() % 40000 + 1000);
				printf("%d: Gender 1 ready to procreate again. \n", p);
			break;
			case 2:
				printf("%d: Gender 2 phase 2\n");
				semdown(procreate2);
				printf("%d: Gender 2 report procreating finished. \n", p);
				usleep(rand() % 40000 + 1000);
				printf("%d: Gender 2 ready to procreate again. \n", p);
			break;
			case 3:
				printf("%d: Gender 3 phase 2\n");
				semdown(nurse);
				usleep(rand() % 200000 + 20000);
				printf("%d: gender 3 report procreating finished. \n", p);
				semup(procreate1);
				semup(procreate2);
				semup(nurse);
				usleep(rand() % 40000 + 1000);
				printf("%d: Gender 3 ready to procreate again. \n", p);
			break;
		}
	}
} 

int main() {
	/* these are aliens waiting for a gathering */
	gender1 = seminit(0, 0);
	gender2 = seminit(0, 0);
	gender3 = seminit(0, 0);
	nurse = seminit(0, 2);
	procreate1 = seminit(0, 0);
	procreate2 = seminit(0, 0);
	lock_init(&mutex);
	
	pid_t pid;
	pid_t opid = getpid();
	int i;
	for(i=0; i<5; i++) {
		pid=fork();
		if(pid == 0) {
			alien(1);
			break;
		} else {
			pid=fork();
			if(pid == 0) {
				alien(2);
				break;
			} else {
				pid=fork();
				if(pid == 0) {
					alien(3);
					break;
				}
			}
		}
	}
	
	if(getpid() == opid) {
		sleep(60);
		semfree(gender1);
		semfree(gender2);
		semfree(gender2);
		semfree(nurse);
		semfree(procreate1);
		semfree(procreate2);
		lock_release(&mutex);
	}
}