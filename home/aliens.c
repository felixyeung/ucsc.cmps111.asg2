#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <lockcond.h>

int main() {
	/* these are aliens waiting for a gathering */
	int gender1 = seminit(0, 0);
	int gender2 = seminit(0, 0);
	int gender3 = seminit(0, 0);
	int nurse = seminit(0, 2);
	int procreate1 = seminit(0, 0);
	int procreate2 = seminit(0, 0);
	
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
	}
}

void alien(int gender) {
	while(1) {
		switch (gender) {
			case 1:
				printf("Gender 1 phase 1\n");
				// if there are others waiting
				if (semvalue(gender2) < 0 && semvalue(gender3) < 0) {
					semup(gender2);
					semup(gender3);
					printf("Gender 1 wakes 2, 3. ready to procreate.\n");
				}
				else {
					//no other genders queued up
					semdown(gender1);
					printf("Cannot pop from 2, 3. 1 going to sleep.\n");
				}
			break;
			case 2:
				printf("Gender 2 phase 1\n");
				if (semvalue(gender1) < 0 && semvalue(gender3) < 0) {
					semup(gender1);
					semup(gender3);
					printf("Gender 2 wakes 1, 3. ready to procreate.\n");
				}
				else {
					//no other genders queued up
					semdown(gender2);
					printf("Cannot pop from 1, 3. 2 going to sleep.\n");
				}
			break;
			case 3:
				printf("Gender 3 phase 1\n");
				if (semvalue(gender1) < 0 && semvalue(gender2) < 0) {
					semup(gender1);
					semup(gender2);
					printf("Gender 3 wakes 1, 2. ready to procreate.\n");
				}
				else {
					//no other genders queued up
					semdown(gender3);
					printf("Cannot pop from 1, 2. 3 going to sleep.\n");
				}
			break;
			
		}
		switch (gender) {
			case 1:
				printf("Gender 1 phase 2\n");
				semdown(procreate1);
				printf("Gender 1 report procreating finished. \n");
				usleep(rand() % 40000 + 1000);
				printf("Gender 1 ready to procreate again. \n");
			break;
			case 2:
				printf("Gender 2 phase 2\n");
				semdown(procreate2);
				printf("Gender 2 report procreating finished. \n");
				usleep(rand() % 40000 + 1000);
				printf("Gender 2 ready to procreate again. \n");
			break;
			case 3:
				printf("Gender 3 phase 2\n");
				semdown(nurse);
				usleep(rand() % 200000 + 20000);
				printf("gender 3 report procreating finished. \n");
				semup(procreate1);
				semup(procreate2);
				semup(nurse);
				printf("Gender 3 ready to procreate again. \n");
			break;
		}
	}
} 