/* Task.h */
#include "Stub.h"
#include "pthread.h"

#define MAX_EVENTS (32)

typedef struct Task
{
  uint32_t name;
  pthread_t pthreadId;
  int rank;
  int epfd;
  int efd[MAX_EVENTS];
}
Task;

PUBLIC Task *Task_create(uint32_t name, void *(*start_routine)(void *));
PUBLIC Task *Task_identify();
PUBLIC Task * Task_findByName(uint32_t name);
