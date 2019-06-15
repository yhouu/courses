#define NTHREAD 8

struct mutex_t {
  int valid;
  struct spinlock lock;
  struct proc *current;
  struct proc *queue[NTHREAD - 1];
  int qsize;
  int qbegin;
  int qend;
};

struct cond_t {
  int active;
  struct spinlock lock;
  struct proc *queue[NTHREAD - 1];
  int qsize;
  int qbegin;
  int qend;
};

