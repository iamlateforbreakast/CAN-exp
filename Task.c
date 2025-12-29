/* Task.c */

#include "Task.h"
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_TASK (5)

static Task *tasks[MAX_TASK];
static int nbTasks = 0;

PUBLIC Task *Task_create(uint32_t name, void *(*start_routine)(void *))
{
  pthread_t handle;
  int epfd;
  int efd;

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
  efd = eventfd(0, EFD_CLOEXEC);
  if (efd == -1)
  {
    printf("eventfd creation failed\n");
    close(epfd);
    return 0;
  }
  Task *task = (Task*)malloc(sizeof(Task));
  task->name = name;
  task->pthreadId = handle;
  task->epfd = epfd;

  tasks[nbTasks] = task;
  nbTasks ++;

  return task;
}

PUBLIC Task * Task_identify()
{
  pthread_t handle = pthread_self();

  for (int i=0; i<MAX_TASK; i++)
  {
    if ((tasks[i]!=0) && (handle == (tasks[i]->pthreadId)))
    {
      return tasks[i];
    }
  }

  return 0;
}

PUBLIC Task * Task_findByName(uint32_t name)
{
  for (int i=0; i<MAX_TASK; i++)
  {
    if ((tasks[i]!=0) && (name == (tasks[i]->name)))
    {
      return tasks[i];
    }
  }

  return 0;
}