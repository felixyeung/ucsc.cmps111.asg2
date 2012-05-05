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
int QUEUESIZE = 10;
int front[100];
int end[100];

/* return true if our process queue is empty */
private int empty(int sem_index) {
	if (front[sem_index] == end[sem_index])
		return 1;
	return 0;
}
 
 /*return 1 if success to append to queue, else return 0*/
 private int push(int sem_index, int value) {
	if ((end[sem_index] + 1) % QUEUESIZE == front[sem_index])
		return 0;
	else {
		//increment end
		end[sem_index] = (end[sem_index] + 1) % QUEUESIZE;
		waiting_procs[sem_index][end[sem_index]] = value;
		return 1;
	}
 }
 
 private int leftpop(int sem_index) {
	int result = NULL;
	if (!empty(sem_index))
		front[sem_index] = (front[sem_index] + 1) % QUEUESIZE;
		result = waiting_procs[sem_index][front[sem_index]];
	}
	return result;
 }


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

private int get_index(int sem, int* semas_identifiers) {
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
TODO: confirm return codes
return 0 on success;
return pid on success and process pop
return -1 if we cant resolve the correct semaphore
*/
public int do_semup(int sem) {
	index = get_index(sem, semas_indentifiers);
	if (index != NULL) {
		semaphores[index] += 1;
		if (semaphores[index] > 0) {
			if (!empty(index)) {
				int pid;
				pid = NULL;
				pid = leftpop(index);
				//TODO:
				//EXPLICITLY WAKE A PROCESS?
				
				//we need to return some kind of result
				int result;
				result = 1234;
				
				//COPIED FROM SETREPLY
				
				/* Fill in a reply message to be sent later to a user process.  System calls
				 * may occasionally fill in other fields, this is only for the main return
				 * value, and for setting the "must send reply" flag.
				 */
				  register struct mproc *rmp = &mproc[pid];

				  if(pid < 0 || pid >= NR_PROCS)
					  panic("setreply arg out of range: %d", pid);

				  rmp->mp_reply.reply_res = result;
				  rmp->mp_flags |= REPLY;	/* reply pending */
				
				
				  //main.c dispatcher will do sendreply() for us
				  
				return pid;
			}
		}
		return 0;
	}
	return -1;
}

public int do_semdown(int sem, struct mproc *rmp) {
	index = get_index(sem, semas_indentifiers);
	if (index != NULL) {
		semaphores[index] - 1;
		if (semaphores[index] < 0) {
			//append caller into queue
			
			//extract pid from object struct to put into queue
			
			//FROM signals.c:do_pause()
			mp->mp_flags |= PAUSED;
			
			return(SUSPEND);
		}
		else {
			//nothing. return jibberish?
			return(100000);
		}
	}
}

public int do_semfree(int sem) {
	index = get_index(sem, semas_indentifiers);
	if (index != NULL) {
		// if queue is empty, free
		if (empty(index)) {
			 sempahores[index] = NULL;
			 semas_indentifiers[index] = NULL;
			 return 1;
		}
	}
	return 0;
}