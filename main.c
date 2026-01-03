#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "Stub.h"
#include "coMgt/CanOpenMgt.h"

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
  sleep(1);
  CanOpenMgt_setCanBusInitialised(TRUE);

  for (;;)
  {
    sleep(1);
  }

  return 0;
}

