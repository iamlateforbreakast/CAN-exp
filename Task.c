/* Task.c */

#include "Task.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include "pthread.h"

#define MAX_TASK (5)

static Task *tasks[MAX_TASK];

PUBLIC Task *Task_create(void *(*start_routine)(void *))
{
  pthread_t handle;
  int epfd;

  int err = pthread_create(&handle, NULL, start_routine, NULL);
  if (err != 0)
  {
    printf("Sync Thread creation failed\n");
    return 0;
  }
  epfd = epoll_create1(EPOLL_CLOEXEC);
  if (epfd == -1)
  {
    printf("epoll_create1 failed\n");
    return 0;
  }
  Task *task = (Task*)malloc(sizeof(Task));
  task->pthreadId = handle;
  task->epfd = epfd;

  return task;
}

Task * Task_identify()
{
  int handle = pthread_self();

  for (int i=0; i<MAX_TASK; i++)
  {
    if (handle == (tasks[i]->pthreadId))
    {
      return tasks[i];
      
    }
  }

  return 0;
}