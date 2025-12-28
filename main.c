#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "Stub.h"

typedef struct TestContext
{
  pthread_cond_t signal10Hz;
  pthread_mutex_t clock10Hz; 
  pthread_cond_t signal200Hz;
  pthread_mutex_t clock200Hz; 
  pthread_cond_t signalSlotPF1;
  pthread_mutex_t clockSlotPF1;
  int epfd;
  int tfd10Hz;
  int tfd200Hz;
  pthread_t syncHandle;
  pthread_t pfBusMgrHandle;
  pthread_t plBusMgrHandle;
  int ticks10Hz;
  int ticks200Hz;
} TestContext;

static TestContext t;

#if 0
void timer10Hz_handler() {
    printf("Timer 10Hz: %d 200Hz: %d\n", t.ticks10Hz, t.ticks200Hz);
    t.ticks10Hz ++;
    RtcCplr_fastRTCSlot = 0;
    pthread_cond_signal(&t.signal10Hz);
}

void timer200Hz_handler() {
    t.ticks200Hz ++;
    RtcCplr_fastRTCSlot++;
    pthread_cond_signal(&t.signal200Hz);
}
#endif

int main()
{
  Stub_init();

  for (;;)
  {
    sleep(1);
  }

  return 0;
}

/*
int efd = eventfd(0, 0);
int epfd = epoll_create1(0);

struct epoll_event ev = { .events = EPOLLIN, .data.fd = efd };
epoll_ctl(epfd, EPOLL_CTL_ADD, efd, &ev);

// Trigger event
uint64_t u = 1;
write(efd, &u, sizeof(u));

// Wait for event
struct epoll_event events[1];
int n = epoll_wait(epfd, events, 1, -1);
*/

