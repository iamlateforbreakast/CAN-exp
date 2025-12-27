/* Task.c */

#include "Task.h"

struct Task
{
  pthread_t pthreadId;
  int rank;
  int epfd;
}
