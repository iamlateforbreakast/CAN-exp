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

T_UINT32 cdhsCyclicWait(T_UINT32 cyclicHdl)
{
  pthread_mutex_lock(&t.clock10Hz);
  pthread_cond_wait(&t.signal10Hz, &t.clock10Hz);
  pthread_mutex_unlock(&t.clock10Hz);
  return 0;
}

Bool RtcCplr_waitForFastRTCSlotEvt(T_UINT32 timeout)
{
  pthread_mutex_lock(&t.clock200Hz);
  pthread_cond_wait(&t.signal200Hz, &t.clock200Hz);
  pthread_mutex_unlock(&t.clock200Hz);

  return 1;
}

// Helper to arm a periodic timerfd
static int arm_timerfd(int tfd, int period_ms) {
    struct itimerspec its = {0};
    its.it_value.tv_sec = period_ms / 1000;
    its.it_value.tv_nsec = (period_ms % 1000) * 1000000;
    its.it_interval = its.it_value; // periodic
    return timerfd_settime(tfd, 0, &its, NULL);
}



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

int main()
{
  t.tfd10Hz = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (t.tfd10Hz == -1) {
    printf("timerfd_create failed\n");
    return -1;
  }
  t.tfd200Hz = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (t.tfd200Hz == -1) {
    printf("timerfd_create failed\n");
    return -1;
  }
  /* Setup 10Hz timer */
  if (arm_timerfd(t.tfd10Hz, 100 /* ms */) == -1) {
    perror("timerfd_settime");
    close(t.tfd10Hz);
    return -1;
  }
  struct epoll_event ev10Hz = {0};
  ev10Hz.events = EPOLLIN;
  ev10Hz.data.u32 = 0;
  if (epoll_ctl(t.epfd, EPOLL_CTL_ADD, t.tfd10Hz, &ev10Hz) == -1) {
    printf("epoll_ctl ADD failed\n");
    close(t.tfd10Hz);
    return -1;
  }
  /* Setup 200Hz timer */
  if (arm_timerfd(t.tfd200Hz, 5 /* ms */) == -1) {
    perror("timerfd_settime");
    close(t.tfd200Hz);
    return -1;
  }
  struct epoll_event ev200Hz = {0};
  ev200Hz.events = EPOLLIN;
  ev200Hz.data.u32 = 0;
  if (epoll_ctl(t.epfd, EPOLL_CTL_ADD, t.tfd200Hz, &ev200Hz) == -1) {
    printf("epoll_ctl ADD failed\n");
    close(t.tfd200Hz);
    return -1;
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

