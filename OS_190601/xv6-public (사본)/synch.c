#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "synch.h"


//  Initialize mutex
int mutex_init(struct mutex_t *mutex){

  if(mutex->valid !=0) return -2; // return-2 : attempting to reinitialize
  if(mutex == 0) return -1; // return-1 : invalid
  
  initlock(mutex->lock,"mutex");
  acquire(mutex->lock);
  mutex->current = 0;
  mutex->valid=1;
  mutex->qsize=0;
  mutex->start =0;
  mutex->end=0;
  release(mutex->lock);
  return 0;  //return 0 : success
}



//  assign lock  
int mutex_lock(struct mutex_t *mutex){
  struct proc *curproc = myproc();
 
  acquire(mutex->lock);
  //return-1 : mutex invalid
  if(mutex == 0){
	release(mutex->lock);
	return -1;
  }

  //return-2 : mutex not initialized
  if(mutex->valid ==0){
	release(mutex->lock);
	return -2;
  }
  
  //return-3 : already has the mutex
  if(mutex->current ==curproc){
  	release(mutex->lock);
	return -3;
  }

  //no one has the mutex
  if(mutex->current==0){
	mutex->current == mutex->queue[mutex->start];
	release(mutex->lock);
	return 0;
  }

  //someone has the mutex
  if(mutex->qsize<NTHREAD-1){
	mutex->queue[mutex->end]=curproc; //enqueue
	//mutex->queue[mutex->end]->state = SLEEPING;  very hard
	mutex->qsize++; //increase q size
	mutex->end=(mutex->end+1) % (NTHREAD-1);
		
	while(mutex->current!= curproc){ //내가 lock을 가지고 있지 않을 때 sleep 
	   sleep(curproc, &mutex->lock);   
	}
  }
  release(mutex->lock);
  return 0;
}



//preassumption: someone already has the lock
int mutex_unlock(struct mutex_t *mutex){
  struct proc *curproc = myproc();
    
  acquire(mutex->lock);
  //return-1 : mutex invalid
  if(mutex == 0){
	release(mutex->lock);
	return -1;
  }

  //return-2 : mutex not initialized
  if(mutex->valid ==0){
	release(mutex->lock);
	return -2;
  }
  
  //return-3 : doesn't have the mutex
  if(mutex->current !=curproc){
  	release(mutex->lock);
	return -3;
  }

  
  //how to unlock?!
  mutex->current = mutex->queue[mutex->start]; //current(at the start point) has the lock
  if(mutex->qsize!=0){
    mutex->queue[mutex->start] =0; //dequeue
    mutex->qsize--; //decrease qsize    
    mutex->start = (mutex->start+1) % (NTHREAD-1);
    wakeup(mutex->current);
  }
  
  release(mutex->lock);
  return 0;
}



//  Initialize CV 
int cond_init(struct cond_t *cond){
  if(cond->active !=0) return -2; // return-2 : attempting to reinitialize
  if(cond == 0) return -1; // return-1 : invalid
  
  initlock(cond->lock,"Condition Variable");
  acquire(cond->lock);
  cond->active=1; //initialized
  cond->qsize=0;
  release(cond->lock);
  return 0;  // return 0 : success
}


//Lock CV
int cond_wait(struct cond_t *cond, struct mutex_t *mutex){
  struct proc *curproc = myproc();
  
  
  acquire(mutex->lock);
  //return-1 : mutex invalid
  if(mutex == 0){
	release(mutex->lock);
	return -1;
  }

  //return-2 : mutex initialized
  if(mutex->valid ==0){
	release(mutex->lock);
	return -2;
  }
  
  //return-3 : already has the mutex
  if(mutex->current ==curproc){
  	release(mutex->lock);
	return -3;
  }
  release(mutex->lock);

  acquire(cond->lock);
  //return-1 : cv invalid
  if(cond == 0){
	release(cond->lock);
	return -1;
  }

  //return-2 : cv initialized
  if(cond->active ==0){
	release(cond->lock);
	return -2;
  }
  
  if(cond->qsize<NTHREAD-1){
	cond->queue[cond->end]=curproc; //enqueue
	mutex_unlock(mutex);
	sleep(curproc, &mutex->lock); 
	cond->qsize++; //increase q size
	cond->end=(cond->end+1) % (NTHREAD-1);
		
/*	while(mutex->current!= curproc){ //내가 lock을 가지고 있지 않을 때 sleep 
	   sleep(curproc, &mutex->lock);   
	}*/
  }

  
  
  release(cond->lock);

}





//unblock a thread
int cond_signal(struct cond_t *cond){
  acquire(cond->lock);
  //return-1 : cv invalid
  if(cond == 0){
	release(cond->lock);
	return -1;
  }

  //return-2 : cv initialized
  if(cond->active ==0){
	release(cond->lock);
	return -2;
  }
  
  
  if(cond->qsize!=0){
    wakeup(cond->queue[cond->start]);
    cond->queue[cond->start] =0; //dequeue
    cond->qsize--; //decrease qsize    
    cond->start = (cond->start+1) % (NTHREAD-1);
  }


  release(cond->lock);
  return 0;
}


