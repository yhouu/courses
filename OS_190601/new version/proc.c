#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "spinlock.h"
#include "synch.h"

struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

static struct proc *initproc;

int nextpid = 1;
int nexttid = 2;
extern void forkret(void);
extern void trapret(void);

static void wakeup1(void *chan);

void
pinit(void)
{
  initlock(&ptable.lock, "ptable");
}

// Must be called with interrupts disabled
int
cpuid() {
  return mycpu()-cpus;
}

// Must be called with interrupts disabled to avoid the caller being
// rescheduled between reading lapicid and running through the loop.
struct cpu*
mycpu(void)
{
  int apicid, i;
  
  if(readeflags()&FL_IF)
    panic("mycpu called with interrupts enabled\n");
  
  apicid = lapicid();
  // APIC IDs are not guaranteed to be contiguous. Maybe we should have
  // a reverse map, or reserve a register to store &cpus[i].
  for (i = 0; i < ncpu; ++i) {
    if (cpus[i].apicid == apicid)
      return &cpus[i];
  }
  panic("unknown apicid\n");
}

// Disable interrupts so that we are not rescheduled
// while reading proc from the cpu structure
struct proc*
myproc(void) {
  struct cpu *c;
  struct proc *p;
  pushcli();
  c = mycpu();
  p = c->proc;
  popcli();
  return p;
}

//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->tid = 1;  // Defalut tid value: 1, main thread
  p->nice = 20;  // Default nice value: 20

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;  // 스택포인터를 끝으로 밀고

  // Leave room for trap frame.
  sp -= sizeof *p->tf;  // 트랩프레임 사이즈만큼(76U) 비워서
  p->tf = (struct trapframe*)sp;  // 스택포인터의 주소를 트랩프레임에게 준다.

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;  // 4만큼 빼서
  *(uint*)sp = (uint)trapret;  // trapret 함수를 담는다.

  sp -= sizeof *p->context;  // context 사이즈만큼(20U) 비워서
  p->context = (struct context*)sp;  // 스택포인터의 주소를 context에게 준다.
  memset(p->context, 0, sizeof *p->context);  // context 0으로 밀어주고
  p->context->eip = (uint)forkret;  // eip는 forkret 함수로.

  return p;
}

// same pid, same nice
static struct proc*
allocthread(int pid, int nice)
{
	struct proc *t;
	char *sp;

	acquire(&ptable.lock);

	for (t = ptable.proc; t < &ptable.proc[NPROC]; t++)
		if (t->state == UNUSED)
			goto found;

	release(&ptable.lock);
	return 0;

found:
	t->state = EMBRYO;
	t->pid = pid;  // same pid with process
	t->tid = nexttid++;
	t->nice = nice;  // same nice value with process

	release(&ptable.lock);

	// Allocate kernel stack.
	if ((t->kstack = kalloc()) == 0) {
		t->state = UNUSED;
		return 0;
	}
	sp = t->kstack + KSTACKSIZE;  // 스택포인터를 끝으로 밀고

	// Leave room for trap frame.
	sp -= sizeof *t->tf;  // 트랩프레임 사이즈만큼(76U) 비워서
	t->tf = (struct trapframe*)sp;  // 스택포인터의 주소를 트랩프레임에게 준다.

	// Set up new context to start executing at forkret,
	// which returns to trapret.
	sp -= 4;  // 4만큼 빼서
	*(uint*)sp = (uint)trapret;  // trapret 함수를 담는다.

	sp -= sizeof *t->context;  // context 사이즈만큼(20U) 비워서
	t->context = (struct context*)sp;  // 스택포인터의 주소를 context에게 준다.
	memset(t->context, 0, sizeof *t->context);  // context 0으로 밀어주고
	t->context->eip = (uint)forkret;  // eip는 forkret 함수로.

	return t;
}

//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}

// Grow current process's memory by n bytes.
// Return 0 on success, -1 on failure.
int
growproc(int n)
{
  uint sz;
  struct proc *curproc = myproc();

  sz = curproc->sz;
  if(n > 0){
    if((sz = allocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  } else if(n < 0){
    if((sz = deallocuvm(curproc->pgdir, sz, sz + n)) == 0)
      return -1;
  }
  curproc->sz = sz;
  switchuvm(curproc);
  return 0;
}

// Create a new process copying p as the parent.
// Sets up stack to return as if from system call.
// Caller must set state of returned proc to RUNNABLE.
int
fork(void)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }

  // Copy process state from proc.
  if((np->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0){
    kfree(np->kstack);
    np->kstack = 0;
    np->state = UNUSED;
    return -1;
  }
  np->sz = curproc->sz;
  np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait() to find out it exited.
void
exit(void)
{
  struct proc *curproc = myproc();
  struct proc *p;
  int fd;

  if(curproc == initproc)
    panic("init exiting");

  // Close all open files.
  for(fd = 0; fd < NOFILE; fd++){
    if(curproc->ofile[fd]){
      fileclose(curproc->ofile[fd]);
      curproc->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(curproc->cwd);
  end_op();
  curproc->cwd = 0;

  acquire(&ptable.lock);

  // Parent might be sleeping in wait().
  wakeup1(curproc->parent);

  // --> terminate whole process and threads.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; ++p){
    if(p->pid == curproc->pid){
      p->state = ZOMBIE;
      wakeup1(p->parent);
    }
  }

  // Pass abandoned children to init.
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->parent == curproc && p->tid == 0){
      cprintf("tid == 0???\n");
      p->parent = initproc;
      if(p->state == ZOMBIE)
        wakeup1(initproc);
    }
  }

  // Jump into the scheduler, never to return.
  curproc->state = ZOMBIE;
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
int
wait(void)
{
  struct proc *p;
  int havekids, pid;
  struct proc *curproc = myproc();
  
  acquire(&ptable.lock);
  for(;;){
    // Scan through table looking for exited children.
    havekids = 0;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->parent != curproc)
        continue;
      havekids = 1;
      if(p->state == ZOMBIE){
        // Found one.
        pid = p->pid;
        kfree(p->kstack);
        p->kstack = 0;
        freevm(p->pgdir);
        p->pid = 0;
        p->parent = 0;
        p->name[0] = 0;
        p->killed = 0;
        p->state = UNUSED;
        release(&ptable.lock);
        return pid;
      }
    }

    // No point waiting if we don't have any children.
    if(!havekids || curproc->killed){
      release(&ptable.lock);
      return -1;
    }

    // Wait for children to exit.  (See wakeup1 call in proc_exit.)
    sleep(curproc, &ptable.lock);  //DOC: wait-sleep
  }
}

//PAGEBREAK: 42
// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run
//  - swtch to start running that process
//  - eventually that process transfers control
//      via swtch back to the scheduler.
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}

// Enter scheduler.  Must hold only ptable.lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->ncli, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  int intena;
  struct proc *p = myproc();

  if(!holding(&ptable.lock))
    panic("sched ptable.lock");
  if(mycpu()->ncli != 1)
    panic("sched locks");
  if(p->state == RUNNING)
    panic("sched running");
  if(readeflags()&FL_IF)
    panic("sched interruptible");
  intena = mycpu()->intena;
  swtch(&p->context, mycpu()->scheduler);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  acquire(&ptable.lock);  //DOC: yieldlock
  myproc()->state = RUNNABLE;
  sched();
  release(&ptable.lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch here.  "Return" to user space.
void
forkret(void)
{
  static int first = 1;
  // Still holding ptable.lock from scheduler.
  release(&ptable.lock);

  if (first) {
    // Some initialization functions must be run in the context
    // of a regular process (e.g., they call sleep), and thus cannot
    // be run from main().
    first = 0;
    iinit(ROOTDEV);
    initlog(ROOTDEV);
  }

  // Return to "caller", actually trapret (see allocproc).
}

// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
void
sleep(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  
  if(p == 0)
    panic("sleep");

  if(lk == 0)
    panic("sleep without lk");

  // Must acquire ptable.lock in order to
  // change p->state and then call sched.
  // Once we hold ptable.lock, we can be
  // guaranteed that we won't miss any wakeup
  // (wakeup runs with ptable.lock locked),
  // so it's okay to release lk.
  if(lk != &ptable.lock){  //DOC: sleeplock0
    acquire(&ptable.lock);  //DOC: sleeplock1
    release(lk);
  }
  // Go to sleep.
  p->chan = chan;
  p->state = SLEEPING;

  sched();

  // Tidy up.
  p->chan = 0;

  // Reacquire original lock.
  if(lk != &ptable.lock){  //DOC: sleeplock2
    release(&ptable.lock);
    acquire(lk);
  }
}

//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan)
      p->state = RUNNABLE;
}

// Wake up all processes sleeping on chan.
void
wakeup(void *chan)
{
  acquire(&ptable.lock);
  wakeup1(chan);
  release(&ptable.lock);
}

// Kill the process with the given pid.
// Process won't exit until it returns
// to user space (see trap in trap.c).
int
kill(int pid)
{
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      p->killed = 1;
      // Wake process from sleep if necessary.
      if(p->state == SLEEPING)
        p->state = RUNNABLE;
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

//PAGEBREAK: 36
// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static char *states[] = {
  [UNUSED]    "unused",
  [EMBRYO]    "embryo",
  [SLEEPING]  "sleep ",
  [RUNNABLE]  "runble",
  [RUNNING]   "run   ",
  [ZOMBIE]    "zombie"
  };
  int i;
  struct proc *p;
  char *state;
  uint pc[10];

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->state == UNUSED)
      continue;
    if(p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    else
      state = "???";
    cprintf("%d %s %s", p->pid, state, p->name);
    if(p->state == SLEEPING){
      getcallerpcs((uint*)p->context->ebp+2, pc);
      for(i=0; i<10 && pc[i] != 0; i++)
        cprintf(" %p", pc[i]);
    }
    cprintf("\n");
  }
}

int
getnice(int pid)
{
  //cprintf("this is getnice func in proc.c file.\n");  //for check

  if (pid < 1) {
    //cprintf("getnice: pid starts from 1.\n");  //for check
    return -1;
  }

  int retval;
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      retval = p->nice;
      //cprintf("nice value is %d\n", p->nice);  //for check
      release(&ptable.lock);
      return retval;
    }
  }
  release(&ptable.lock);
  return -1;
}

int
setnice(int pid, int value)
{
  //cprintf("this is setnice func in proc.c file.\n");  //for check

  if (pid < 1) {
    //cprintf("setnice: pid starts from 1.\n");  //for check
    return -1;
  }

  if (value < 0 || value > 39) {
    //cprintf("setnice input value out of range.\n");  //for check
    return -1;
  }

  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid == pid){
      //cprintf("nice value is changed from %d ", p->nice);  //for check
      p->nice = value;
      //cprintf("to %d\n", p->nice);  //for check
      release(&ptable.lock);
      return 0;
    }
  }
  release(&ptable.lock);
  return -1;
}

void
ps(int pid)
{
  //cprintf("this is ps func in proc.c file.\n");  //for check

  if (pid < 0) return;
  struct proc *p;

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid < 1) continue;
    if(!pid || p->pid == pid){
      cprintf("pid: %d, tid: %d\n", p->pid, p->tid);
      cprintf("name: %s\n", p->name);
      cprintf("nice: %d\n", p->nice);
      //cprintf("state: %d\n", p->state);  //enum converting complete
      cprintf("state: ");
      switch (p->state)
      {
      case UNUSED:
        cprintf("UNUSED\n");
      break;
      case EMBRYO:
        cprintf("EMBRYO\n");
      break;
      case SLEEPING:
        cprintf("SLEEPING\n");
      break;
      case RUNNABLE:
        cprintf("RUNNABLE\n");
      break;
      case RUNNING:
        cprintf("RUNNING\n");
      break;
      case ZOMBIE:
        cprintf("ZOMBIE\n");
      break;
      default:
        cprintf("unknown. Something's going wrong.\n");
      break;
      }

      if (pid) break;
    }
  }
  release(&ptable.lock);
  return;
}

int
thread_create(void *(*function)(void*), void *arg, void *stack)
{
	/*
	Create a new thread at calling process

	Return value
	• Return the thread ID(tid) of the new thread
	• If err, return -1


	• The new thread starts execution in invoking
	function.
	• Arg is passed as the argument of function.
	• Stack is the pointer to call stack of new thread.
	• All threads in a process have same pid & priority.
	• Initial thread in a process is a main thread which
	has tid ‘1’.
	• A process can have maximum 8 threads.
	*/
	//cprintf("Here is thread_create.\n");  //for check

	int i, tcount;
	struct proc *newt;
	struct proc *curproc = myproc();

	// Check maximum thread 8
	// newt is meaningless here. Just a pointer for looping ptable.
	tcount = 0;
	acquire(&ptable.lock);
	for (newt = ptable.proc; newt < &ptable.proc[NPROC]; ++newt) {
		if (newt->pid == curproc->pid) {
			++tcount;
		}
	}
	release(&ptable.lock);
	if (tcount >= 8) {  // already there are 8 threads
		return -1;
	}

	// Allocate process.  --> Allocate thread.
	if ((newt = allocthread(curproc->pid, curproc->nice)) == 0) {
		return -1;
	}

	//// Copy process state from proc.  --> don't.
	//if ((newt->pgdir = copyuvm(curproc->pgdir, curproc->sz)) == 0) {
	//	kfree(newt->kstack);
	//	newt->kstack = 0;
	//	newt->state = UNUSED;
	//	return -1;
	//}

	newt->pgdir = curproc->pgdir;  // Threads share pgdir with process.

	newt->sz = curproc->sz;
	newt->parent = curproc;  // Does process become parent of threads?
	*newt->tf = *curproc->tf;

	// Clear %eax so that fork returns 0 in the child.
	newt->tf->eax = 0;

	// Setting function, arg, stack
	newt->tf->eip = (uint)function;
	//newt->tf->edi = (uint)arg;  // doesn't use edi?
	newt->tf->ebp = (uint)stack + 4096 - 8;  // user should give stack size of 4096
	//*(uint*)newt->tf->ebp = curproc->tf->ebp;  // save old ebp - 여기서 하는게 아닌가...
	*(int*)newt->tf->ebp = -1;  // return address, but threads never return.
	*(uint*)(newt->tf->ebp + 4) = (uint)arg;  // first parameter
	newt->tf->esp = newt->tf->ebp;

	for (i = 0; i < NOFILE; i++)
		if (curproc->ofile[i])
			newt->ofile[i] = filedup(curproc->ofile[i]);
	newt->cwd = idup(curproc->cwd);

	safestrcpy(newt->name, curproc->name, sizeof(curproc->name));

	acquire(&ptable.lock);

	newt->state = RUNNABLE;

	release(&ptable.lock);

	return newt->tid;
}

void
thread_exit(void *retval)
{
	/*
	• Synopsis
	  • Terminate the calling thread
	  • void thread_exit(void *retval)

	• Each thread save retval at thread_exit().
	• Thread state transfers to ZOMBIE.
	• Thread resources are retrieved at thread_join.
	• Exiting thread may wake up threads which have a
	same pid.
	*/
	//cprintf("Here is thread_exit.\n");  //for check

	struct proc *curthread = myproc();  //this is thread
	struct proc *p;  //thread?
	int fd;

	if (curthread == initproc)
		panic("init exiting");

	// Close all open files.
	for (fd = 0; fd < NOFILE; fd++) {
		if (curthread->ofile[fd]) {
			fileclose(curthread->ofile[fd]);
			curthread->ofile[fd] = 0;
		}
	}

	begin_op();
	iput(curthread->cwd);
	end_op();
	curthread->cwd = 0;

	// put retval into tf->eax
	curthread->tf->eax = (uint)retval;

	acquire(&ptable.lock);

	// Parent might be sleeping in wait().  --> don't
	//wakeup1(curthread->parent);

	//  --> instead, same pid all thread shoule be waken up.

	//for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
	//  if (p->pid == curthread->pid && p->tid != curthread->tid) {
	//    wakeup1(p);
	//  }
	//}  // --> do it in below ptable searching.

	// Pass abandoned children to init.
	for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
		if (p->pid == curthread->pid && p->tid != curthread->tid) {
			wakeup1(p);  // --> awake other threads.
		}
		if (p->parent == curthread) {
			p->parent = initproc;
			if (p->state == ZOMBIE)
				wakeup1(initproc);
		}
	}

	// Jump into the scheduler, never to return.
	curthread->state = ZOMBIE;
	//cprintf("thread_exit end part. call sched().\n");  //for check
	sched();
	panic("zombie exit");
}

int
thread_join(int tid, void **retval)
{
	/*
	• Synopsis
	  • Join with terminated thread
	  • thread_join(int tid, void **retval)

	• Return value
	  • If success, return 0
	  • If there’s no thread with input tid, return -1

	• Wait thread specified with tid to terminate.
	• Caller may sleep until corresponding thread terminated.
	• If thread has already terminated, return immediately.
	• Copy the exit status of the target thread into the
	location pointed to by retval.
	• The call stack of the terminated thread should be
	freed by the calling thread.
	*/
	//cprintf("Here is thread_join.\n");  //for check

	struct proc *t;  //thread
	int havethreads;
	struct proc *curproc = myproc();  //process

	acquire(&ptable.lock);
	for (;;) {
		// Scan through table looking for exited children.
		// --> exited thread.
		havethreads = 0;
		for (t = ptable.proc; t < &ptable.proc[NPROC]; ++t) {
			if (t->pid != curproc->pid || t->tid != tid)
				continue;
			havethreads = 1;
			if (t->state == ZOMBIE) {
				// Found thread.
				*retval = (void*)t->tf->eax;  //get retval from eax.
				kfree(t->kstack);
				t->kstack = 0;
				//freevm(t->pgdir);  --> don't. Threads share process's pgdir.
				t->pid = 0;
				t->tid = 0;
				t->parent = 0;
				t->name[0] = 0;
				t->killed = 0;
				t->state = UNUSED;
				release(&ptable.lock);
				//cprintf("thread_join end.\n");  //for check
				return 0;
			}
		}

		// Failed to find such thread or killed.
		if (!havethreads || curproc->killed) {
			release(&ptable.lock);
			return -1;
		}

		// Wait for thread to exit.  (See wakeup1 call in proc_exit.)
		sleep(curproc, &ptable.lock);  //DOC: wait-sleep
	}
}

int
getsome(int* arg)
{
  struct proc *t = myproc();
  acquire(&ptable.lock);

  cprintf("pid: %d", t->pid);
  cprintf(", tid: %d", t->tid);
  cprintf(", arg: %d", *arg);
  cprintf(", &arg: %p\n", arg);

  cprintf("tf: %p", t->tf);
  cprintf(", &tf->edi: %p\n", &t->tf->edi);

  cprintf("ebp: %p", t->tf->ebp);
  cprintf(", esp: %p", t->tf->esp);
  cprintf(", edi: %p", t->tf->edi);
  cprintf(", &edi: %p\n", &t->tf->edi);

  cprintf("*ebp: %p", *(uint*)t->tf->ebp);
  cprintf(", *ebp+4: %p", *(uint*)(t->tf->ebp+4));
  cprintf(", *ebp+8: %p\n", *(uint*)(t->tf->ebp+8));

  cprintf("context(%p):\n", t->context);
  cprintf("edi: %p", t->context->edi);
  cprintf(", esi: %p", t->context->esi);
  cprintf(", ebx: %p", t->context->ebx);
  cprintf(", ebp: %p", t->context->ebp);
  cprintf(", eip: %p\n", t->context->eip);

  cprintf("*ebp: %p", *(uint*)t->context->ebp);
  cprintf(", *eip: %p\n", *(uint*)t->context->eip);

  cprintf("\n");
  release(&ptable.lock);
  return 0;
}

