#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_halt(void)
{
  outw(0x604, 0x2000);
  return 0;
}


int 
sys_getnice(void)
{
	int pid;
	
   if(argint(0,&pid)<0)   
	   return -1;

   return getnice(pid);
   
}

int 
sys_setnice(void)
{
	int pid, value;

	if(argint(0,&pid)<0) 
		return -1;
	if(argint(1,&value)<0) 
		return -1;

	return setnice(pid, value);
}

int 
sys_ps(void)
{
	int pid;

	if(argint(0,&pid)<0)  
	   	return -1;

	ps(pid);
	return 0;
}

int
sys_thread_create(void)
{
  int function, args, stack;

  if(argint(0, &function) <0) return -1;
  if(argint(1, &args) <0) return -1;
  if(argint(2, &stack) <0) return -1;

  return thread_create((void*)function, (void*)args, (void*)stack);
}

int
sys_thread_join(void)
{
	int tid;
	void *retval=0;
	if(argint(0,&tid)<0) return -1;
	if(argptr(1,(char**)&retval, sizeof(void**))<0) return -1;

	return thread_join(tid, retval);
}

int
sys_thread_exit(void)
{
  void *retval;
  
  if(argptr(0,(char**)&retval,sizeof(retval))<0)
	  return -1;
  thread_exit(retval);
  return 0;
}

int
sys_gettid(void)
{
  return gettid();
}

int 
sys_mutex_init(void)
{
  struct mutex_t* mutex;
  if(argint(0,&mutex)<0) return -1;
  if(mutex==0) return -1;
  return mutex_init(mutex);
}

int 
sys_mutex_lock(void)
{
  struct mutex_t* mutex;
  if(argint(0,&mutex)<0) return -1;
  if(mutex==0) return -1;
  return mutex_lock(mutex);
}

int 
sys_mutex_unlock(void)
{
  struct mutex_t* mutex;
  if(argint(0,&mutex)<0) return -1;
  if(mutex==0) return -1;  //machangaji
  return mutex_unlock(mutex);
}

int 
sys_cond_init(void)
{
  struct cond_t* cond;
  if(argint(0,&cond)<0) return -1;
  if(cond==0) return -1;
  return cond_init(cond);
}

int 
sys_cond_wait(void)
{
  struct cond_t* cond;
  struct mutex_t* mutex;
  if(argint(0,&cond)<0) return -1;
  if(cond==0) return -1;
  if(argint(1,&mutex)<0) return -1;
  if(mutex==0) return -1;

  return cond_wait(cond,mutex);
}

int 
sys_cond_signal(void)
{
  struct cond_t* cond;
  if(argint(0,&cond)<0) return -1;
  if(cond==0) return -1;
  return cond_signal(cond);
}
