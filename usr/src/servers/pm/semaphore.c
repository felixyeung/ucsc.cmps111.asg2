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
#include <lib.h>
#include <string.h>
#include <errno.h>
#include "mproc.h"
#include "param.h"

/*101 because 100 entries and we don't use index 0*/
static int semaphores[101];
static unsigned int semas_identifiers[101]; /*semas_identifiers[i]==0 means i is an unused index*/
static int waiting_procs[101][1024]; /*only 10 waiting processes per semaphore*/
static int QUEUESIZE = 1024;
static int front[101];
static int end[101];

/* return true if our process queue is empty */
PRIVATE int empty(int sem_index) {
	if (front[sem_index] == end[sem_index])
		return 1;
	return 0;
}
 
 /*return 1 if success to append to queue, else return 0*/
 /*circular array*/
 PRIVATE int push(int sem_index, int value) {
	if ((end[sem_index] + 1) % QUEUESIZE == front[sem_index])
		return 0;
	else {
		//increment end
		end[sem_index] = (end[sem_index] + 1) % QUEUESIZE;
		waiting_procs[sem_index][end[sem_index]] = value;
		return 1;
	}
 }
 
 /*returns 0 if queue is empty (failure)
 returns who_p otherwise*/
 PRIVATE int leftpop(int sem_index) {
	int result = NULL;
	if (!empty(sem_index)) {
		front[sem_index] = (front[sem_index] + 1) % QUEUESIZE;
		result = waiting_procs[sem_index][front[sem_index]];
	}
	return result;
 }

/*returns first free index
0 if failure*/
PRIVATE int find_first_free() {
	int i;
	for (i = 1; i < 101;  i++) {
		//remember to unset semas_identifier in semfree() so we can do this
		if (semas_identifiers[i] == 0) {
			return i;
		}
	}
	return NULL;
}

/*returns 1 if identifier is in use, 0 if not in use*/
PRIVATE int is_in_use(int sem) {
	int i;
	for (i = 1; i < 101;  i++) {
		if (semas_identifiers[i] == sem) {
			return 1;
		}
	}
	return 0;
}

/*returns index of a semaphore
given input of the identifier
returns 0 on failure to find index*/
PRIVATE int get_index(int sem) {
	if(sem <= 0)
		return 0; /*no nonpositive identifiers*/
	int i;
	for (i = 1; i < 101;  i++) {
		if (semas_identifiers[i] == sem) {
			return i;
		}
	}
	return 0;
}

/*initializes a new semaphore,
returns the identifier if no error*/
PUBLIC int do_seminit(void) {
	/*return EINVAL;
	return who_p;*/
	int identifier;
	identifier = m_in.m1_i1;
	
	int value;
	value = m_in.m1_i2;
	int index;
	if(value < -1000 || value > 1000) {
		return EINVAL; /*add to errno.h*/
	}
	if(0 == (index = find_first_free()))
		return EAGAIN;
	if(index == NULL) {
		return EAGAIN;
	}
	if(identifier > 0) {
		if (is_in_use(identifier)) {
			return EEXIST;
		}
		semas_identifiers[index] = identifier;
		semaphores[index] = value;
		return identifier;
	} else if(identifier == 0) {
		unsigned int i;
		int name = NULL;
		for(int i=1; i < 2147483648; i++) {
			if(! is_in_use(i)) {
				name = i;
				break;
			}
		}
		if(name == NULL) {
			return EAGAIN;
		}
		semas_identifiers[index] = name;
		semaphores[index] = value;
		return name;
	} else {
		return EINVAL; /*negative identifier*/
	}
	
}

PUBLIC int do_semvalue(void) {
	int identifier;
	identifier = m_in.m1_i1;

	int index;
	index = get_index(identifier);
	if (index != NULL)
		return semaphores[index] + 1000000;
	//else, return some error
	return EINVAL;
}

/*
Resolve index from identifier, increment this semaphore by one.
if there's a waiting proc in queue, pop it and wake it.
TODO: confirm return codes
return 0 on success;
return pid on success and process pop
return -1 if we cant resolve the correct semaphore
*/
PUBLIC int do_semup(void) {
	int identifier;
	identifier = m_in.m1_i1;
	
	int index;
	index = get_index(identifier);
	if (index != NULL) {
		semaphores[index] += 1; /*increment the value of the semaphore*/
		if (semaphores[index] <= 0) { /*if the new value is less than or equal to zero, we need to wake something up from the queue*/
			//take a process from the queue
			int queue_proc = NULL;
			if(NULL == (queue_proc = leftpop(index))
				return 0; /*this is not good... empty queue? but why.  hopefully not gonna happen.*/
			setreply(queue_proc, 1);
			sendreply();
		}
		return 1;
	}
	return 0; /*fail due to invalid identifier*/
}


/*INSTEAD OF PASSING IN A MPROC AS AN ARGUMENT, WE JUST WANT TO RECEIVE A MESSAGE OF THE PID*/

PUBLIC int do_semdown(void) { 
	int identifier;
	identifier = m_in.m1_i1;

	/*since this is called from PM's main.c, who_p is necessarily valid*/
	printf("who_p = %d\n", who_p);
	
	int index;
	index = get_index(identifier);
	if (index != NULL) {
		semaphores[index] -= 1;
		if (semaphores[index] < 0) {
			//append caller into queue
			
			//extract pid from object struct to put into queue for semaphores[index]
			if (0 == (push(index, who_p)))
				return 0; /*maximum processes are waiting on this semaphore*/
			
			//FROM signals.c:do_pause()
			mp->mp_flags |= PAUSED;
			
			return(SUSPEND);
		}
		else {
			//success. return positive jibberish.
			return(1);
		}
	}
	//semaphore not initialized.
	return 0;
}

PUBLIC int do_semfree() {
	int identifier;
	identifier = m_in.m1_i1;
	
	int index;
	index = get_index(identifier);
	if (index != NULL) {
		// if queue is empty, free
		if (empty(index)) {
			 semaphores[index] = NULL;
			 semas_identifiers[index] = NULL;
			 return 1;
		}
	}
	return 0;
}
