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

rtems_status_code rtems_task_ident (
  rtems_name name,
  unsigned32 node,
  Objects_Id *id)
  {
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
  Task *t = Task_identify();
  
  // Wait for event
  //struct epoll_event events[1];
  //int n = epoll_wait(epfd, events, 1, -1);

  return 0;
}

rtems_status_code rtems_event_send (
  Objects_Id          id,
  rtems_event_set    event_in
)
{
  //struct epoll_event ev = { .events = EPOLLIN, .data.fd = efd };
  //epoll_ctl(epfd, EPOLL_CTL_ADD, efd, &ev);

  // Trigger event
  //uint64_t u = 1;
  //write(efd, &u, sizeof(u));
  return 0;
}

rtems_status_code rtems_clock_get (
  rtems_clock_get_options   option,
  void                     *time_buffer
)
{
  return 0;
}

void * rtcCplr_taskBody(void * p)
{
  Task *self = Task_identify();

  int tfd10Hz;
  int tfd200Hz;

  /* Initialisation */
  struct epoll_event ev10Hz = {0};
  ev10Hz.events = EPOLLIN;
  ev10Hz.data.u32 = 0;
  if (epoll_ctl(self->epfd, EPOLL_CTL_ADD, tfd10Hz, &ev10Hz) == -1) {
    printf("epoll_ctl ADD failed\n");
    close(tfd10Hz);
    return 0;
  }

  /* Main loop */
  struct epoll_event events[2];
  for (;;)
  {
    int n = epoll_wait(self->epfd, events, 2, -1);
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
        RtcCplr_fastRTCSlot = 0;
        //timer10Hz_handler(); Signal 10Hz
      }
      uint64_t expirations200Hz = 0;
      r = read(tfd200Hz, &expirations200Hz, sizeof(expirations200Hz));
      if (r == sizeof(expirations200Hz) && expirations200Hz > 0) {
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
  Task_create(&rtcCplr_taskBody); /* RtcCplr */
  Task_create(&syncTaskBody); /* Sync */
  Task_create(&pfBusMgrTaskBody); /* PF Bus Mgr */
  Task_create(&plBusMgrTaskBody); /* PL Bus Mgr */
}
