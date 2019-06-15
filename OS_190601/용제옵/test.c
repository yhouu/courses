#include "types.h"
#include "stat.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "proc.h"
#include "user.h"
#include "synch.h"

#define N 8

void *stack[NTHREAD];
int tid[NTHREAD];
void *retval[NTHREAD];

struct mutex_t m;
struct cond_t c;
int buffer[10];
int idx=0;
int condition = 0;

void *thread(void *arg){
	mutex_lock(&m);
	printf(1, "t[%d] got mutex, start.\n", arg);

	while(condition == 0){
		cond_wait(&c, &m);
	}

	condition--;
	buffer[idx++] = (int)arg;
	printf(1, "t[%d] condition %d -> %d, end, call mutex_unlock.\n", arg, condition + 1, condition);
	mutex_unlock(&m);
	thread_exit(0);
}

int
main(int argc, char **argv)
{
	int i;

	printf(1, "TEST: ");

	for(i=0;i<NTHREAD;i++)
		stack[i] = malloc(4096);
	mutex_init(&m);
	cond_init(&c);

	printf(1, "boom check 1.\n");

	for(i=0;i<NTHREAD-1;i++){
		tid[i] = thread_create(thread, (void *)i, stack[i]);

		if(tid[i] == -1){
			printf(1, "thread create failed\n");
			exit();
		}
	}

	printf(1, "boom check 2.\n");

	for(i=0;i<NTHREAD-1;i++){
		mutex_lock(&m);
		condition++;
		printf(1, "main condition %d -> %d.\n", condition - 1, condition);
		cond_signal(&c);
		mutex_unlock(&m);
	}

	printf(1, "boom check 3.\n");

	for(i=0;i<NTHREAD-1;i++){
		if(thread_join(tid[i], &retval[i]) == -1){
			printf(1, "thread join failed\n");
			exit();
		}
	}

	for(i=0;i<NTHREAD-1;i++){
		printf(1, "buffer[%d] = %d\n", i, buffer[i]);  // check
		if(buffer[i] != i){
			printf(1, "WRONG\n");
			//exit();
		}
	}

	for(i=0;i<NTHREAD;i++)
		free(stack[i]);

	printf(1, "OK\n");

	exit();
}
