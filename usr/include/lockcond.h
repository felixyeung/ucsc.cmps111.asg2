#include <lib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sema.h>

typedef struct {
	//this holds a semaphore identifier
	int mutex;
	int next;
	
	//lol, this is really an int
	int nextCount;
} lock;

typedef struct {
	lock *l;
	//this is a semaphore
	int condSem;
	
	//this is really an int
	int semCount = 0;
} cond;

PUBLIC void lock_init(struct lock* l) {
	l.mutex = seminit(0, 1);
	l.next = seminit(0, 0);
	
	l.nextCount = 0;
}

PUBLIC void lock_acquire(struct lock* l) {
	semdown(l.mutex);
}

PUBLIC void lock_release(struct lock* l) {
	if (l.nextCount > 0)
		semup(l.next);
	else
		semup(l.mutex);
}

PUBLIC void cond_init(struct cond* c, struct lock* l) {
	lock_init(l)
	c.l = l;
	c.condSem = seminit(0,0);
	
	c.semCount = 0;
}

PUBLIC int cond_wait(struct cond* c) {
	c.semCount += 1;
	if (c.l.nextCount > 0)
		semup(c.l.next);
	else
		semup(c.l.mutex);
	c.condSem.down();
	c.semCount -= 1;
}

PUBLIC int cond_signal (struct cond* c) {
	if (c. semCount > 0) {
		c.l.nextCount += 1;
		c.(condSem);
		semdown(c.l.next);
		c.l.nextCount -= 1;
	}
}