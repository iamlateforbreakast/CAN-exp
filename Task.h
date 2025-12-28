/* Task.h */
#include "Stub.h"
#include "pthread.h"

typedef struct Task
{
  pthread_t pthreadId;
  int rank;
  int epfd;
}
Task;

PUBLIC Task *Task_create();
PUBLIC Task *Task_identify();
