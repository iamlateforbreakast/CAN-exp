#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "Stub.h"

typedef struct TestContext
{
  pthread_cond_t signal10Hz;
  pthread_mutex_t clock10Hz; 
  pthread_cond_t signal200Hz;
  pthread_mutex_t clock200Hz;
} TestContext;

static TestContext t;

int main()
{
  Stub_init();

  for (;;)
  {
    sleep(1);
  }

  return 0;
}

