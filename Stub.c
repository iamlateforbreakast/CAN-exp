#include "Stub.h"
#include "Task.h"

RtcContext RtcCplr_syncObt;
U32 RtcCplr_fastRTCSlot = 0;

typedef struct StubContext
{
  Task *tasks[5];
} StubContext;

StubContext s;

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
  Task *t = Task_identify(s.tasks);
  
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

void Stub_init()
{
  s.tasks[0] = Task_create(); /* RtcCplr */
  s.tasks[1] = Task_create(); /* Sync */
  s.tasks[2] = Task_create(); /* PF Bus Mgr */
  s.tasks[3] = Task_create(); /* PL Bus Mgr */
}
