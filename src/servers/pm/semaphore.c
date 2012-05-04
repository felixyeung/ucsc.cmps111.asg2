#include "pm.h"
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <minix/callnr.h>
#include <minix/endpoint.h>
#include <minix/com.h>
#include <minix/vm.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/sigcontext.h>
#include <string.h>
#include <errno.h>
#include "mproc.h"
#include "param.h"


int semaphores[100];
unsigned int semas_identifiers[100];
int waiting_procs[100][10]; /*only 10 waiting processes per semaphore*/

private int find_first_free(int* semas_identifiers) {
	int i;
	int result;
	result = NULL;
	for (i = 0; i < 100;  i++) {
		//remember to unset semas_indentifier in semfree() so we can do this
		if (semas_indentifiers[i] == NULL) {
			result = i;
			break;
		}
	}
	return result;
}

private int is_in_use(int sem, int* semas_identifiers) {
	int i;
	int result;
	//0 = is not in use
	//1 = is in use;
	result = 0;
	for (i = 0; i < 100;  i++) {
		if (semas_indentifiers[i] == sem) {
			result = 1;
			break;
		}
	}
	return result;
}

private int get_index(int sem, int* semas_identifiers);
	int i;
	int result;
	result = NULL;
	for (i = 0; i < 100;  i++) {
		if (semas_indentifiers[i] == sem) {
			result = i;
			break;
		}
	}
	return result;
}

public int do_seminit(int sem, int value) {
	int index;
	if(abs(value) > 1000) {
		return EINVAL; /*add to errno.h*/
	}
	index = find_first_free(semas_identifiers);
	if(index == NULL) {
		return EAGAIN;
	}
	if(sem > 0) {
		if is_in_use(sem, semas_identifiers) {
			return EEXIST;
		}
		semas_identifiers[index] = sem;
		semaphores[index] = value;
		return sem;
	} else if(sem == 0) {
		unsigned int i;
		int name = NULL;
		for(int i=1; i < 2147483648; i++) {
			if(! is_in_use(i, semas_identifiers)) {
				name = i;
				break;
			}
		}
		if(name == NULL) {
			return EAGAIN;
		}
		semas_identifiers[index] = name;
		semaphore[index] = value;
		return name;
	} else {
		return EINVAL; /*negative identifier*/
	}
	
}

public int do_semvalue(int sem) [
	index = get_index(sem, semas_indentifiers);
	if (index != NULL)
		return semaphores[index];
	//else, return some error
}

/*
Resolve index from indentifier, increment this semaphore by one.
if there's a waiting proc in queue, pop it and wake it.
*/
public int do_semup(int sem) {
	index = get_index(sem, semas_indentifiers);
	if (index != NULL) {
		semaphores[index] += 1;
		if (semaphores[index] > 0) {
			//if (!empty(waiting_procs[index])) {
				//TODO:
				//p = popleft(waiting_procs[index]);
				//wake(p);
			//}
		}
	}
	//else fail
}

public int do_semdown(int sem) {
	index = get_index(sem, semas_indentifiers);
	if (index != NULL) {
		semaphores[index] - 1;
		if (semaphores[index] < 0) {
			//append caller into queue
			// wait(caller);
			return(SUSPEND);
		}
		else {
			//nothing. return jibberish?
			return(100000);
		}
	}
}

public int do_semfree(int sem);