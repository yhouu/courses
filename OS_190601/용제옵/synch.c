#include "types.h"
#include "defs.h"
#include "param.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "synch.h"

int
cond_in_queue_test(struct cond_t *cond, struct proc *p)
{
	// Validation check skip.
	int flag = 0;
	int i;
	for (i = 0; i < NTHREAD - 1; ++i) {
		if (cond->queue[i] == p) {
			flag = 1;
		}
	}
	return flag;
}

int
mutex_init(struct mutex_t *mutex)
{
	if (mutex->valid == 1) {  // already initialized.
		cprintf("mutex_init already initialized.\n");
		return -2;
	}

	initlock(&mutex->lock, "mutex");

	acquire(&mutex->lock);

	memset(mutex->queue, 0, sizeof(mutex->queue));
	mutex->current = 0;
	mutex->valid = 1;
	mutex->qsize = 0;
	mutex->qbegin = 0;
	mutex->qend = 0;

	release(&mutex->lock);
	return 0;
}

// The mutex lock must be held.
int
mutex_lock1(struct mutex_t *mutex)
{
	struct proc *curproc = myproc();

	//if (!mutex->valid) {
	//	cprintf("mutex_lock mutex invalid.\n");
	//	return -2;
	//}
	//acquire(&mutex->lock);

	if (mutex->current == curproc) {
		cprintf("mutex_lock double mutex.\n");
		//release(&mutex->lock);
		return -3;
	}
	if (mutex->current == 0 && mutex->qsize == 0) {  // mutex is free.
		mutex->current = curproc;
		//release(&mutex->lock);
		return 0;
	}

	// Enqueue.
	if (mutex->qsize >= NTHREAD - 1) {  // full, but the queue will not be full.
		cprintf("mutex queue is full.\n");
		panic("mutex queue is full.");  //
		//release(&mutex->lock);
		return -4;
	}
	++mutex->qsize;
	mutex->queue[mutex->qend] = curproc;
	mutex->qend = (mutex->qend + 1) % (NTHREAD - 1);
	while (mutex->current != curproc) {
		sleep(curproc, &mutex->lock);
	}
	//release(&mutex->lock);
	return 0;
}

int
mutex_lock(struct mutex_t *mutex)
{
	int ret;
	if (!mutex->valid) {
		cprintf("mutex_lock mutex invalid.\n");
		return -2;
	}
	acquire(&mutex->lock);
	ret = mutex_lock1(mutex);
	release(&mutex->lock);
	return ret;
}

// The mutex lock must be held.
int
mutex_unlock1(struct mutex_t *mutex)
{
	struct proc *curproc = myproc();
	struct proc *next;

	//if (!mutex->valid) {
	//	cprintf("mutex_unlock mutex invalid.\n");
	//	return -2;
	//}
	//acquire(&mutex->lock);

	if (mutex->current != curproc) {  // not having mutex.
		cprintf("mutex_unlock not having mutex.\n");
		//release(&mutex->lock);
		return -3;
	}

	if (mutex->qsize == 0) {  // no one waiting mutex.
		mutex->current = 0;
		//release(&mutex->lock);
		return 0;
	}

	// Dequeue first come thread. Give mutex.
	--mutex->qsize;
	next = mutex->queue[mutex->qbegin];
	mutex->queue[mutex->qbegin] = 0;
	mutex->qbegin = (mutex->qbegin + 1) % (NTHREAD - 1);
	mutex->current = next;
	wakeup(next);
	//release(&mutex->lock);
	return 0;
}

int
mutex_unlock(struct mutex_t *mutex)
{
	int ret;
	if (!mutex->valid) {
		cprintf("mutex_unlock mutex invalid.\n");
		return -2;
	}
	acquire(&mutex->lock);
	ret = mutex_unlock1(mutex);
	release(&mutex->lock);
	return ret;
}

int
cond_init(struct cond_t *cond)
{
	if (cond->active == 1) {  // already initialized.
		cprintf("cond_init already initialized.\n");
		return -2;
	}

	initlock(&cond->lock, "cond");

	acquire(&cond->lock);

	memset(cond->queue, 0, sizeof(cond->queue));
	cond->active = 1;
	cond->qsize = 0;
	cond->qbegin = 0;
	cond->qend = 0;

	release(&cond->lock);
	return 0;
}

// Should be called with mutex locked.
int
cond_wait(struct cond_t *cond, struct mutex_t *mutex)
{
	// Mutex is locked.
	struct proc *curproc = myproc();

	if (!mutex->valid) {
		cprintf("cond_wait mutex invalid.\n");
		return -2;
	}
	if (mutex->current != curproc) {
		cprintf("cond_wait not having mutex.\n");
		return -3;
	}

	if (!cond->active) {
		cprintf("cond_wait cond inactive.\n");
		return -2;
	}
	acquire(&cond->lock);

	// Enqueue.
	if (cond->qsize >= NTHREAD - 1) {  // full, but the queue will not be full.
		cprintf("condition variable queue is full.\n");
		cprintf("%d / %d %d %d %d %d %d %d\n", curproc->tid, cond->queue[0]->tid, cond->queue[1]->tid, cond->queue[2]->tid, cond->queue[3]->tid, cond->queue[4]->tid, cond->queue[5]->tid, cond->queue[6]->tid);  // for debugging
		panic("condition variable queue is full.");  //
		release(&cond->lock);
		return -4;
	}
	++cond->qsize;
	cond->queue[cond->qend] = curproc;
	cond->qend = (cond->qend + 1) % (NTHREAD - 1);
	//cprintf("%d / %d %d %d %d %d %d %d\n", curproc->tid, cond->queue[0]->tid, cond->queue[1]->tid, cond->queue[2]->tid, cond->queue[3]->tid, cond->queue[4]->tid, cond->queue[5]->tid, cond->queue[6]->tid);  // for debugging
	release(&cond->lock);
	acquire(&mutex->lock);
	mutex_unlock1(mutex);
	do {
		sleep(curproc, &mutex->lock);
		// Threads can be woken up by the thread_exit of other threads.
	} while (cond_in_queue_test(cond, curproc));
	mutex_lock1(mutex);
	release(&mutex->lock);
	return 0;
}

// Should be called with mutex locked???
int
cond_signal(struct cond_t *cond)
{
	// Mutex locked???
	struct proc *curproc = myproc();
	struct proc *next;

	if (!cond->active) {
		cprintf("cond_signal cond inactive.\n");
		return -2;
	}
	acquire(&cond->lock);

	if (cond->qsize == 0) {  // no one waiting condition.
		release(&cond->lock);
		return 0;
	}

	// Dequeue first come thread. Wake up.
	--cond->qsize;
	next = cond->queue[cond->qbegin];
	cond->queue[cond->qbegin] = 0;
	cond->qbegin = (cond->qbegin + 1) % (NTHREAD - 1);
	//cprintf("%d %d %d %d %d %d %d / %d\n", cond->queue[0]->tid, cond->queue[1]->tid, cond->queue[2]->tid, cond->queue[3]->tid, cond->queue[4]->tid, cond->queue[5]->tid, cond->queue[6]->tid, next->tid);  // for debugging
	wakeup(next);
	release(&cond->lock);
	return 0;
}

