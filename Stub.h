#ifndef _STUB_H_
#define _STUB_H_
#include <stdint.h>
#include <stdio.h>

#define TRUE (1)
#define FALSE (0)

#define PUBLIC
#define PRIVATE static
#define IMPORT extern

/* From io/coMgt/CanOpenCommon.h */
#define U16_MSK (0xFFFF)

typedef uint8_t U08;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint32_t T_BOOL;

typedef uint32_t T_UINT32;
typedef unsigned char Bool;

typedef uint32_t Uint;

/* From infra/util/basicTypes.h */
typedef char Char;

/* From rtems/types.h */
typedef uint32_t unsigned32;
typedef unsigned32 rtems_id;
typedef uint32_t rtems_name;
typedef unsigned32 Objects_Id;
typedef unsigned32 rtems_interval;

/* From rtems.h */
#define RTEMS_SEARCH_LOCAL_NODE (0x7FFFFFFF)
#define RTEMS_NO_TIMEOUT (0)

/* From retms/options.h */
#define RTEMS_WAIT 0x00000000
#define RTEMS_EVENT_ANY 0x00000002
typedef unsigned32 rtems_option;
typedef enum
{
  RTEMS_CLOCK_GET_TOD,
  RTEMS_CLOCK_GET_SECONDS_SINCE_EPOCH,
  RTEMS_CLOCK_GET_TICKS_SINCE_BOOT,
  RTEMS_CLOCK_GET_TICKS_PER_SECOND,
  RTEMS_GET_TIME_VALUE
} rtems_clock_get_options;

/* From rtems/status.h */
typedef enum{
  RTEMS_SUCCESSFUL = 0
} rtems_status_code;

/* From rtems/eventset.h */
typedef unsigned32 rtems_event_set;

/* From rtems/tasks.h */
#define RTEMS_SELF 0
typedef unsigned32 rtems_task_argument;
rtems_status_code rtems_task_ident (
  rtems_name name,
  unsigned32 node,
  Objects_Id *id);
rtems_status_code rtems_task_suspend(
  Objects_Id id
);

/* From rtems/event.h */
rtems_status_code rtems_event_receive (
  rtems_event_set event_in,
  rtems_option    option_set,
  rtems_interval  ticks,
  rtems_event_set *event_out
);
rtems_status_code rtems_event_send (
  Objects_Id          id,
  rtems_event_set    event_in
);

/* From rtems/clock.h */
rtems_status_code rtems_clock_get (
  rtems_clock_get_options   option,
  void                     *time_buffer
);

/* From infra/include/cdhsTypes.h */
#ifndef PARAM_NOT_USED
#define PARAM_NOT_USED(param) ((void)(param))
#endif

/* From infra/include/errorLib.h */
#define SW_ERROR (0x01)
#define ERROR_REPORT(level, data1, data2, data3) \
  do \
  { \
    printf("Error\n"); \
  } while (0)

typedef struct RtcContext
{
  unsigned int itCurSlot; // 0-19
  unsigned int coarseOBT;
  unsigned int subSeconds;
} RtcContext;

IMPORT RtcContext RtcCplr_syncObt;

/* From io/rtcCplr/RtcCplr.h */
#define FAST_RTC_SLOTS_IN_RTC (20)
PUBLIC Bool RtcCplr_registerTaskForFastRTC(rtems_event_set evt);
PUBLIC U32 RtcCplr_getFastRTCSlot(void);
PUBLIC Bool RtcCplr_waitForFastRTCSlotEvt(T_UINT32 timeout);

IMPORT void cdhsCyclicInit(void);
IMPORT T_UINT32 cdhsCyclicRegister(T_UINT32 period, T_UINT32 phase, T_UINT32 deadline);
IMPORT T_UINT32 cdhsCyclicWait(T_UINT32 cyclicHdl);

/* From infra/include/initLib.h */
IMPORT void cdhsInitWait(void);

IMPORT U32 RtcCplr_fastRTCSlot;

#endif /* _STUB_H_ */