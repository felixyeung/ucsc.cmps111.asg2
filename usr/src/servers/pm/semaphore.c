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

<<<<<<< HEAD

int semaphores[100];
unsigned int semas_identifiers[100];
int waiting_procs[100][10]; /*only 10 waiting processes per semaphore*/
int QUEUESIZE = 10;
int front[100];
int end[100];
=======
static int semaphores[101];
static unsigned int semas_identifiers[101]; /*semas_identifiers[i]==0 means i is an unused index*/
static int waiting_procs[101][1024]; /*only 10 waiting processes per semaphore*/
static int QUEUESIZE = 1024;
static int front[101];
static int end[101];
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5

/* return true if our process queue is empty */
PRIVATE int empty(int sem_index) {
	if (front[sem_index] == end[sem_index])
		return 1;
	return 0;
}
 
 /*return 1 if success to append to queue, else return 0*/
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
 
 PRIVATE int leftpop(int sem_index) {
	int result = NULL;
	if (!empty(sem_index)) {
		front[sem_index] = (front[sem_index] + 1) % QUEUESIZE;
		result = waiting_procs[sem_index][front[sem_index]];
	}
	return result;
 }


<<<<<<< HEAD
PRIVATE int find_first_free(int* semas_identifiers) {
	int i;
	int result;
	result = NULL;
	for (i = 0; i < 100;  i++) {
		//remember to unset semas_identifier in semfree() so we can do this
		if (semas_identifiers[i] == NULL) {
			result = i;
			break;
=======
PRIVATE int find_first_free() {
	int i;
	int result;
	result = NULL;
	for (i = 1; i < 101;  i++) {
		//remember to unset semas_identifier in semfree() so we can do this
		if (semas_identifiers[i] == 0) {
			result = i;
			printf("discovered free\n");
			return result;
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
		}
	}
	return result;
}

<<<<<<< HEAD
PRIVATE int is_in_use(int sem, int* semas_identifiers) {
	int i;
	int result;
	//0 = is not in use
	//1 = is in use;
	result = 0;
	for (i = 0; i < 100;  i++) {
		if (semas_identifiers[i] == sem) {
			result = 1;
			break;
		}
	}
	return result;
=======
/*returns 1 if identifier is in use, 0 if not in use*/
PRIVATE int is_in_use(int sem) {
	int i;
	for (i = 1; i < 101;  i++) {
		if (semas_identifiers[i] == sem) {
			return 1;
		}
	}
	return 0;
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
}

PRIVATE int get_index(int sem, int* semas_identifiers) {
	int i;
	int result;
	result = NULL;
<<<<<<< HEAD
	for (i = 0; i < 100;  i++) {
=======
	for (i = 1; i < 101;  i++) {
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
		if (semas_identifiers[i] == sem) {
			result = i;
			break;
		}
	}
	return result;
}

<<<<<<< HEAD
PUBLIC int do_seminit(void) {
=======
/*initializes a new semaphore,
returns the identifier if no error*/
PUBLIC int do_seminit(void) {
	/*return EINVAL;
	return who_p;*/
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
	int identifier;
	identifier = m_in.m1_i1;
	
	int value;
	value = m_in.m1_i2;
<<<<<<< HEAD
	
	int index;
	if(value < -1000 || value > 1000) {
		return EINVAL; /*add to errno.h*/
	}
	index = find_first_free(semas_identifiers);
	if(index == NULL) {
		return EAGAIN;
	}
	if(identifier > 0) {
		if (is_in_use(identifier, semas_identifiers)) {
			return EEXIST;
		}
=======
printf("identifier=%d, value=%d\n", identifier, value);	
	int index;
	if(value < -1000 || value > 1000) {
printf("!!! 1 !!!\n");
		return EINVAL; /*add to errno.h*/
	}
	index = find_first_free();
printf("index=%d\n", index);
	if(index == NULL) {
		return EAGAIN;
	}
printf("passed case where eagain is return when no free index is found\n");
	if(identifier > 0) {
printf("we are in the case where identifier is >0\n");
		if (is_in_use(identifier)) {
			return EEXIST;
		}
printf("identifier is not in use\n");
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
		semas_identifiers[index] = identifier;
		semaphores[index] = value;
		return identifier;
	} else if(identifier == 0) {
		unsigned int i;
		int name = NULL;
		for(int i=1; i < 2147483648; i++) {
<<<<<<< HEAD
			if(! is_in_use(i, semas_identifiers)) {
=======
			if(! is_in_use(i)) {
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
				name = i;
				break;
			}
		}
		if(name == NULL) {
			return EAGAIN;
		}
		semas_identifiers[index] = name;
		semaphores[index] = value;
<<<<<<< HEAD
		return 0;
	} else {
=======
		return name;
	} else {
printf("!!! 2 !!! \n");
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
		return EINVAL; /*negative identifier*/
	}
	
}

PUBLIC int do_semvalue(void) {
	int identifier;
	identifier = m_in.m1_i1;

	int index;
	index = get_index(identifier, semas_identifiers);
	if (index != NULL)
<<<<<<< HEAD
		return 0;
=======
		return semaphores[index] + 1000000;
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
	//else, return some error
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
	index = get_index(identifier, semas_identifiers);
	if (index != NULL) {
		semaphores[index] += 1;
		if (semaphores[index] > 0) {
			if (!empty(index)) {
				
				//take a process from the queue
				int piddie;
				piddie = NULL;
				piddie = leftpop(index);
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
				  register struct mproc *rmp = &mproc[piddie];

				  if(piddie < 0 || piddie >= NR_PROCS)
					  panic("setreply arg out of range: %d", piddie);

				  rmp->mp_reply.reply_res = result;
				  rmp->mp_flags |= REPLY;	/* reply pending */
				
				
				  //main.c dispatcher will do sendreply() for us
				  
<<<<<<< HEAD
				//ret piddie
				return 0;
			}
		}
		//we can increment, there are no procs in queue
=======
				return piddie;
			}
		}
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
		return 0;
	}
	return -1;
}


/*INSTEAD OF PASSING IN A MPROC AS AN ARGUMENT, WE JUST WANT TO RECEIVE A MESSAGE OF THE PID*/

PUBLIC int do_semdown(void) { 
	int identifier;
	identifier = m_in.m1_i1;

	register struct mproc* rmp = &mproc[m_in.m1_i2];
	
	int piddie;
	piddie = rmp->mp_pid;
	
	int index;
	index = get_index(identifier, semas_identifiers);
	if (index != NULL) {
		semaphores[index] -= 1;
		if (semaphores[index] < 0) {
			//append caller into queue
			
			//extract pid from object struct to put into queue for semaphores[index]
			push(index, piddie);
			
			//FROM signals.c:do_pause()
			mp->mp_flags |= PAUSED;
			
			return(SUSPEND);
		}
		else {
			//nothing. return jibberish?
<<<<<<< HEAD
			return 0;
		}
	}
	return -1;
=======
			return(100000);
		}
	}
	return(-1234);
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
}

PUBLIC int do_semfree() {
	int identifier;
	identifier = m_in.m1_i1;
	
	int index;
	index = get_index(identifier, semas_identifiers);
	if (index != NULL) {
		// if queue is empty, free
		if (empty(index)) {
			 semaphores[index] = NULL;
			 semas_identifiers[index] = NULL;
<<<<<<< HEAD
			 return 0;
		}
	}
	return -1;
}

PUBLIC int do_semtest() {
	int incoming_message;
	incoming_message = m_in.m1_i1;
	
	if (incoming_message == 1)
		return 0;
	else if (incoming_message == 0)
		return SUSPEND;
	else
		return EINVAL;
}
=======
			 return 1;
		}
	}
	return 0;
}
>>>>>>> bfa4d5bf1ac9684b0a1fa80b78f43ea1fc6729f5
