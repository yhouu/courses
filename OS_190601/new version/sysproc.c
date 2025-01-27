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

  if(argint(0, &pid) < 0)
    return -1;
  return getnice(pid);
}

int
sys_setnice(void)
{
  //cprintf("this is sys_setnice func in sysproc.c file.\n");  //for check
  int pid, value;

  if(argint(0, &pid) < 0) return -1;
  if(argint(1, &value) < 0) return -1;
  //cprintf("pid: %d, value: %d\n", pid, value);  //for check
  return setnice(pid, value);
}

int
sys_ps(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  ps(pid);
  return 0;
}

int
sys_thread_create(void)
{
  //cprintf("Here is sys_thread_create.\n");  //for check

  void *(*function)(void*);
  void *arg;
  void *stack;

  if(argint(0, &function) < 0) return -1;
  if(argint(1, &arg) < 0) return -1;
  if(argint(2, &stack) < 0) return -1;
  return thread_create(function, arg, stack);
}

int
sys_thread_exit(void)
{
  //cprintf("Here is sys_thread_exit.\n");  //for check

  void *retval;

  if(argint(0, &retval) < 0) return -1;
  thread_exit(retval);
  return 0;
}

int
sys_thread_join(void)
{
  //cprintf("Here is sys_thread_join.\n");  //for check

  int tid;
  void **retval;

  if(argint(0, &tid) < 0) return -1;
  if(argint(1, &retval) < 0) return -1;
  return thread_join(tid, retval);
}

int
sys_gettid(void)
{
  //cprintf("Here is sys_gettid.\n");  //for check
  return myproc()->tid;
}

int
sys_mutex_init(void)
{
  //cprintf("Here is sys_mutex_init.\n");  //for check
  struct mutex_t *mutex;
  if(argint(0, &mutex) < 0) {
    cprintf("ss1\n");
    return -1;
  }
  if (mutex == 0) {
    cprintf("ss2\n");
    return -1;
  }
  return mutex_init(mutex);
}

int
sys_mutex_lock(void)
{
  //cprintf("Here is sys_mutex_lock.\n");  //for check
  struct mutex_t *mutex;
  if(argint(0, &mutex) < 0) {
    cprintf("ss3\n");
    return -1;
  }
  if (mutex == 0) {
    cprintf("ss4\n");
    return -1;
  }
  return mutex_lock(mutex);
}

int
sys_mutex_unlock(void)
{
  //cprintf("Here is sys_mutex_unlock.\n");  //for check
  struct mutex_t *mutex;
  if(argint(0, &mutex) < 0) {
    cprintf("ss5\n");
    return -1;
  }
  if (mutex == 0) {
    cprintf("ss6\n");
    return -1;
  }
  return mutex_unlock(mutex);
}

int
sys_cond_init(void)
{
  //cprintf("Here is sys_cond_init.\n");  //for check
  struct cond_t *cond;
  if(argint(0, &cond) < 0) {
    cprintf("ss7\n");
    return -1;
  }
  if (cond == 0) {
    cprintf("ss8\n");
    return -1;
  }
  return cond_init(cond);
}

int
sys_cond_wait(void)
{
  //cprintf("Here is sys_cond_wait.\n");  //for check
  struct cond_t *cond;
  struct mutex_t *mutex;
  if(argint(0, &cond) < 0) {
    cprintf("ss9\n");
    return -1;
  }
  if(argint(1, &mutex) < 0) {
    cprintf("ss10\n");
    return -1;
  }
  if (cond == 0 || mutex == 0) {
    cprintf("ss11\n");
    return -1;
  }
  return cond_wait(cond, mutex);
}

int
sys_cond_signal(void)
{
  //cprintf("Here is sys_cond_signal.\n");  //for check
  struct cond_t *cond;
  if(argint(0, &cond) < 0) {
    cprintf("ss12\n");
    return -1;
  }
  if (cond == 0) {
    cprintf("ss13\n");
    return -1;
  }
  return cond_signal(cond);
}

int
sys_asdf(void)
{
  cprintf("Here is sys_asdf.\n");  //for check
  return mutex_init(0);
}

