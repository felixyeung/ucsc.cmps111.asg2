/*
semaphore.c
*/

#include <sys/cdefs.h>
#include "namespace.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <lib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>

int seminit (int sem, int value) {
	return 1;
}

int semvalue (int sem) {
	return 1;
}

int semup (int sem) {
	return 1;
}

int semdown (int sem) {
	return 1;
}

int semfree (int sem) {
	return 1;
}
