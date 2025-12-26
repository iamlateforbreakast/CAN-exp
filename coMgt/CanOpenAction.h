#ifndef _CanOpenAction_
#define _CanOpenAction_

#include "Stub.h"
#include "coMgt/CoMgtHk.h"
#include "coMgt/CanOpenCommon.h"

#define CO_BUS_NAME_LENGTH (8)

typedef struct CoCtx
{
  Char busName[CO_BUS_NAME_LENGTH];
  Uint busId;
  U16 evtPid;
  rtems_id taskId;

  CoState curState;

  Uint cycles;
  Uint slotInCycle;
  Uint slots;
  Uint itInCycle;

  U08 actPmHealth;
  U08 extCanHealth;
  U16 dummy00;

  U32 cucC32B;
  U16 cucF16B;
} CoCtx;

IMPORT CoCtx CoMgr_ctx[ROV_CANBUS_NUM];

PUBLIC CoStatus CanOpenAction_setup(CoCtx *pCtx, U16 evtPid);
PUBLIC void CanOpenAction_updateHkArea(CoCtx *pCtx, CoState entryState);
#endif /* _CanOpenAction_ */