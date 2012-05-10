#include <stdio.h>
#include <stdlib.h>
#include <sema.h>

int main() {
	int i,j,k=0;
	for(i=1; 1; i++) {
		while((j = semfree(i)) != 1 && k<=1000) {
			semup(i);
			k++;
		}
		if(k != 1000)
			printf("semfree(%d)=1\n", i);
		else
			printf("semfree(%d)=0\n", i);
		k=0;
		sleep(1);
	}
	return 0;
}
