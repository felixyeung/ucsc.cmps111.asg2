#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sema.h>

typedef struct {
	//this holds a semaphore identifier
	int mutex;
} lock;

PUBLIC int lock_init(struct lock* l) {
	l.mutex = seminit(0, 1);
	return l.mutex
}

PUBLIC int lock_acquire(struct lock* l) {
	return semdown(l.mutex);
}

PUBLIC int lock_release(struct lock* l) {
	return semup(l.mutex);
}