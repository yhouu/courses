#define NTHREAD 8

struct mutex_t{
  int valid; //valid=0 uninitialized, valid=1 initialized
  struct spinlock *lock;
  struct proc *current;
  struct proc *queue[NTHREAD-1]; //waiting queue
  int qsize; //queue size
  int start;
  int end;
};

struct cond_t{
  int active; //active=0 uninitialized, active=1 initialized
  struct spinlock *lock;
  struct proc *queue[NTHREAD-1];
  int qsize;
  int start;
  int end;
};
