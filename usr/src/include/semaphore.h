#include <lib.h>
#include <unistd.h>
#include <sys/types.h>

PUBLIC int seminit(int sem, int value) {
	message m;

	m.m1_i1 = sem;
	m.m1_i2 = value;

	_syscall(PM_PROC_NR, 103, &m);
}

PUBLIC int semvalue(int sem) {
	message m;

	m.m1_i1 = sem;

	_syscall(PM_PROC_NR, 105, &m);
}

PUBLIC int semup(int sem) {
	message m;

	m.m1_i1 = sem;

	_syscall(PM_PROC_NR, 108, &m);
}

PUBLIC int semdown(int sem) {
	message m;

	m.m1_i1 = sem;
	m.m1_i2 = getpid();

	_syscall(PM_PROC_NR, 109, &m);
}

PUBLIC int semfree(int sem) {
	message m;

	m.m1_i1 = sem;

	_syscall(PM_PROC_NR, 110, &m);
}
