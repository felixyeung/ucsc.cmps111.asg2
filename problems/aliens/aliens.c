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
}

void alien(int gender) {
	while(1) {
		switch (gender) {
			case 1:
				// if there are others waiting
				if (semvalue(gender2) < 0 && semvalue(gender3) < 0) {
					semup(gender2);
					semup(gender3);
				}
				else {
					//no other genders queued up
					semdown(gender1);
				}
			break;
			case 2:
				if (semvalue(gender1) < 0 && semvalue(gender3) < 0) {
					semup(gender1);
					semup(gender3);
				}
				else {
					//no other genders queued up
					semdown(gender2);
				}
			break;
			case 3:
				if (semvalue(gender1) < 0 && semvalue(gender2) < 0) {
					semup(gender1);
					semup(gender2);
				}
				else {
					//no other genders queued up
					semdown(gender3);
				}
			break;
		}
		switch (gender) {
			case 1:
				semdown(procreate1);
				usleep(rand() % 40000 + 1000);
			break;
			case 2:
				semdown(procreate2);
				usleep(rand() % 40000 + 1000);
			break;
			case 3:
				// this will always be executed before case 1
				// because case 1
				semdown(nurse);
				usleep(rand() % 200000 + 20000);
				semup(procreate1);
				semup(procreate2);
				semup(nurse);
			break;
		}
	}
} 