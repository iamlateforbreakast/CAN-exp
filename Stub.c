#include "Stub.h"
#include "Task.h"

#include "coMgt/CanOpenMgt.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <errno.h>

RtcContext RtcCplr_syncObt;
U32 RtcCplr_fastRTCSlot = 0;

void cdhsCyclicInit(void)
{

}

Bool RtcCplr_registerTaskForFastRTC(rtems_event_set evt)
{
  return FALSE;
}

U32 RtcCplr_getFastRTCSlot(void)
{
  return RtcCplr_fastRTCSlot;
}

T_UINT32 cdhsCyclicRegister(T_UINT32 period, T_UINT32 phase, T_UINT32 deadline)
{
  return 0;
}

void cdhsInitWait(void)
{

}

T_UINT32 cdhsCyclicWait(T_UINT32 cyclicHdl)
{
  //pthread_mutex_lock(&t.clock10Hz);
  //pthread_cond_wait(&t.signal10Hz, &t.clock10Hz);
  //pthread_mutex_unlock(&t.clock10Hz);
  return 0;
}

Bool RtcCplr_waitForFastRTCSlotEvt(T_UINT32 timeout)
{
  //pthread_mutex_lock(&t.clock200Hz);
  //pthread_cond_wait(&t.signal200Hz, &t.clock200Hz);
  //pthread_mutex_unlock(&t.clock200Hz);

  return 1;
}

rtems_status_code rtems_task_ident (
  rtems_name name,
  unsigned32 node,
  Objects_Id *id)
  {
    if (name == 0) {
      Task *self = Task_identify();
      *id = self->name;
    } else {
      *id = 0; // TBC
    }
    return 0;
  }

rtems_status_code rtems_task_suspend(
  Objects_Id id
)
{
  return 0;
}

rtems_status_code rtems_event_receive (
  rtems_event_set event_in,
  rtems_option    option_set,
  rtems_interval  ticks,
  rtems_event_set *event_out
)
{
  Task *self = Task_identify();
  
  // Wait for event
  struct epoll_event events[32];
  int n = epoll_wait(self->epfd, events, 1, -1);

  for (int i=0; i<n; i++) {
    int fd = events[i].data.fd;
    for (int j =1; j<=MAX_EVENTS; j << 1)
    {
      if ((event_in & j) && (fd == self->efd[j]))
      {
        // Consume event
        uint64_t u;
        read(fd, &u, sizeof(u));
        *event_out = j;
        return 0;
      }
    }
  }
  return 0;
}

rtems_status_code rtems_event_send (
  Objects_Id          id,
  rtems_event_set    event_in
)
{
  Task *t = Task_findByName(id);
  for (int i =1; i<=MAX_EVENTS; i << 1)
  {
    if (event_in & i)
    {
      // Trigger event
      uint64_t u = 1;
      write(t->efd[i], &u, sizeof(u));
    }
  }
  
  return 0;
}

rtems_status_code rtems_clock_get (
  rtems_clock_get_options   option,
  void                     *time_buffer
)
{
  return 0;
}

// Helper to arm a periodic timerfd
static int arm_timerfd(int tfd, int period_ms) {
    struct itimerspec its = {0};
    its.it_value.tv_sec = period_ms / 1000;
    its.it_value.tv_nsec = (period_ms % 1000) * 1000000;
    its.it_interval = its.it_value; // periodic
    return timerfd_settime(tfd, 0, &its, NULL);
}

void * rtcCplr_taskBody(void * p)
{
  Task *self = Task_identify();

  if (!self) return 0;
  
  int tfd10Hz;
  int tfd200Hz;

  /* Initialisation */
  tfd10Hz = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (tfd10Hz == -1) {
    printf("timerfd_create failed\n");
    return 0;
  }
  /* Setup 10Hz timer */
  if (arm_timerfd(tfd10Hz, 100 /* ms */) == -1) {
    perror("timerfd_settime");
    close(tfd10Hz);
    return 0;
  }

  struct epoll_event ev10Hz = {0};
  ev10Hz.events = EPOLLIN;
  ev10Hz.data.u32 = 0;
  if (epoll_ctl(self->epfd, EPOLL_CTL_ADD, tfd10Hz, &ev10Hz) == -1) {
    printf("epoll_ctl ADD failed\n");
    close(tfd10Hz);
    return 0;
  }

  tfd200Hz = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (tfd200Hz == -1) {
    printf("timerfd_create failed\n");
    return 0;
  }
  /* Setup 200Hz timer */
  if (arm_timerfd(tfd200Hz, 5 /* ms */) == -1) {
    perror("timerfd_settime");
    close(tfd200Hz);
    return 0;
  }
  struct epoll_event ev200Hz = {0};
  ev200Hz.events = EPOLLIN;
  ev200Hz.data.u32 = 0;
  if (epoll_ctl(self->epfd, EPOLL_CTL_ADD, tfd200Hz, &ev200Hz) == -1) {
    printf("epoll_ctl ADD failed\n");
    close(tfd200Hz);
    return 0;
  }
  /* Main loop */
  struct epoll_event events[2];
  for (;;)
  {
    int n = epoll_wait(self->epfd, events, 1, -1);
    if (n < 0) {
      if (errno == EINTR) continue;
        printf("epoll_wait\n");
        break;
    }
    for (int i = 0; i < n; i++) {
      int idx = events[i].data.u32;

      uint64_t expirations10Hz = 0;
      ssize_t r = read(tfd10Hz, &expirations10Hz, sizeof(expirations10Hz));
      if (r == sizeof(expirations10Hz) && expirations10Hz > 0) {
        printf("RTC 10Hz\n");
        RtcCplr_fastRTCSlot = 0;
        //timer10Hz_handler(); Signal 10Hz
      }
      uint64_t expirations200Hz = 0;
      r = read(tfd200Hz, &expirations200Hz, sizeof(expirations200Hz));
      if (r == sizeof(expirations200Hz) && expirations200Hz > 0) {
        printf("RTC 200Hz\n");
        RtcCplr_fastRTCSlot++;
        // timer200Hz_handler(); Signal 200Hz
      }
    }
  }
  return 0;
}

void * syncTaskBody(void * p)
{
  CanOpenMgt_syncTaskBody(0);
  return 0;
}

void * pfBusMgrTaskBody(void * p)
{
  CanOpenMgt_busMgr(0);
  return 0;
}

void * plBusMgrTaskBody(void * p)
{
  CanOpenMgt_busMgr(1);
  return 0;
}

void Stub_init()
{
  Task_create(1, &rtcCplr_taskBody); /* RtcCplr */
  Task_create(2, &syncTaskBody); /* Sync */
  //Task_create(3, &pfBusMgrTaskBody); /* PF Bus Mgr */
  //Task_create(&plBusMgrTaskBody); /* PL Bus Mgr */
}
