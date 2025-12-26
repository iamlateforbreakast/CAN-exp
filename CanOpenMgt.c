#include "../Stub.h"
#include "coMgt/CanOpenCommon.h"
#include "coMgt/CanOpenAction.h"
#include "coMgt/CanOpenMgt.h"
#include "coMgt/CanOpenBus.h"

#define COMGT_FAST_RTC_EVENT (0x40000000)

/* All events received by Bus manager tasks */
#define ALL_SYNC_EVENTS (0xFFFFFFFF)

/* Build a different event according bus ID and slot number
 * 0xFC000000 -> 2
 * 0xFFFFFFFF -> 20 bits for 20 different IT interrupts */
#define EVENT_FROM_BUS_SLOT(bus, interruptSlot) (((1<<(22+((bus)&1)))&0xFC000000) | ((1<<(interruptSlot))&0xFFFFF))

typedef struct RtcSyncCtx
{
  Bool syncTaskActive;
  rtems_id syncTaskId;
  Uint cycleCount;
  Uint curSlot;
  Uint itCurSlot;
  Uint coarseTime32B;
  U16 fineTime16B;
} RtcSyncCtx;

U32 CoMgr_ticksPerSecond;

T_BOOL CoMgr_canBusInitialised = FALSE;

T_BOOL CoMgr_isFirstActivation = TRUE;

PRIVATE RtcSyncCtx rtcSyncCtx =
{
  .syncTaskActive = FALSE,
  .syncTaskId = 0,
  .cycleCount = 0,
  .curSlot = 0,
  .itCurSlot = 0,
  .coarseTime32B = 0,
  .fineTime16B = 0
};

PRIVATE void cycleScetGet(U32 *pCoarse32B, U16 *pFineU16B);
PRIVATE Bool busMgrRestart(void);

void CanOpenMgt_init(U16 evtPid)
{
  Uint bus;
  CoCtx *pCtx;

  rtems_clock_get(RTEMS_CLOCK_GET_TICKS_PER_SECOND, (void*)&CoMgr_ticksPerSecond);
  /* setup automaton start-up context for all CAN busses */
  for (bus = 0; bus < ROV_CANBUS_NUM; bus++)
  {
    pCtx = &CoMgr_ctx[bus];
    (void)CanOpenAction_setup(pCtx, evtPid);
  }

  /* Initialise RTC Coupler interface context */
  rtcSyncCtx.syncTaskActive = FALSE;
  rtcSyncCtx.syncTaskId = 0;
  rtcSyncCtx.cycleCount = 0;
  rtcSyncCtx.curSlot = 0;
  rtcSyncCtx.itCurSlot = 0;
  rtcSyncCtx.coarseTime32B = 0;
  rtcSyncCtx.fineTime16B = 0;
}

void CanOpenMgt_syncTaskBody(rtems_task_argument unused)
{
  Uint cyclicHdl;
  Bool waitRTC = TRUE;
  Bool go = TRUE;

  PARAM_NOT_USED(unused);

  /* retrieve its own task Id */
  (void) rtems_task_ident (RTEMS_SELF, RTEMS_SEARCH_LOCAL_NODE, &rtcSyncCtx.syncTaskId);

  /* store it in the RTC synchronisation context */
  rtcSyncCtx.syncTaskActive = TRUE;

  /* Wait for the end of the initialisation of all the tasks */
  cdhsInitWait();

  /* Register to the cyclic manager */
  cyclicHdl = cdhsCyclicRegister(1, 0, 1);

  /* Register to the RTC coupler to receive Fast RTC notifications */
  RtcCplr_registerTaskForFastRTC(COMGT_FAST_RTC_EVENT);

  waitRTC = TRUE;
  while (go)
  {
    /* Wait for Correct timing Event */
    if (waitRTC == TRUE)
    {
      /* first slot: Started by SW cycle at 10 Hz */

      /* Waiting for the cycle activation: expected as 10HZ (100 ms) */
      cdhsCyclicWait(cyclicHdl);

      /* update counters */
      rtcSyncCtx.cycleCount++;
      rtcSyncCtx.curSlot = 0;
      rtcSyncCtx.itCurSlot = 0;
      
      /* update the SCET time */
      cycleScetGet(&rtcSyncCtx.coarseTime32B, &rtcSyncCtx.fineTime16B);
      
      /* Waiting for Fast RTC */
      waitRTC = FALSE;
    }
    else
    {
      /* Following slots: started by Fast RTC */

      /* Wait for the slot activation */
      go = RtcCplr_waitForFastRTCSlotEvt(RTEMS_NO_TIMEOUT);

      if (go)
      {
        rtcSyncCtx.itCurSlot = RtcCplr_getFastRTCSlot();
        if ((rtcSyncCtx.itCurSlot >0) && (rtcSyncCtx.itCurSlot % 2 == 0))
        {
          rtcSyncCtx.curSlot = (rtcSyncCtx.itCurSlot / 2);
        }
        if (rtcSyncCtx.itCurSlot == (FAST_RTC_SLOTS_IN_RTC - 1))
        {
          waitRTC = TRUE;
        }
      }
      else
      {
        printf("CanOpenMgt_syncTaskBody: Invalid Fast RTC slot %u\n", rtcSyncCtx.itCurSlot);
        ERROR_REPORT(SW_ERROR, rtcSyncCtx.itCurSlot, rtcSyncCtx.curSlot, 0);
      }
    }
    if (go)
    {
      if (CoMgr_canBusInitialised == TRUE)
      {
        if ((CoMgr_isFirstActivation == FALSE) || (rtcSyncCtx.itCurSlot ==0))
        {
          CoMgr_isFirstActivation = FALSE;
          go = busMgrRestart();
        }
      }
    }
  }

  /* Suspended to be analysed or deleted */
  rtems_task_suspend(RTEMS_SELF);
}

void CanOpenMgt_busMgr(rtems_task_argument busId)
{
  CoCtx *pCtx;
  rtems_status_code rtStatus;
  rtems_event_set rtEvtOut;
  rtems_event_set expRtEvt;
  Bool go = TRUE;
  Uint slotToWait;

  if ((busId != CANBUS_ID_PF) && (busId != CANBUS_ID_PL))
  {
    printf("CanOpenMgt_busMgr: Invalid bus ID %u\n", busId);
    ERROR_REPORT(SW_ERROR, busId, 0, 0);
    pCtx = NULL;
    go = FALSE;
  }
  else
  {
    /* initialise its context pointer */
    pCtx = &CoMgr_ctx[busId];

    (void)rtems_task_ident (RTEMS_SELF, RTEMS_SEARCH_LOCAL_NODE, &pCtx->taskId);

    /* prepare the first incoming event */
    slotToWait = 0;

    /* wait for the first Slot (0) */
    rtEvtOut = 0;
    expRtEvt = EVENT_FROM_BUS_SLOT(busId, slotToWait);
    rtStatus = rtems_event_receive (expRtEvt, RTEMS_WAIT|RTEMS_EVENT_ANY,
                                    RTEMS_NO_TIMEOUT, &rtEvtOut);
    if ((rtStatus != RTEMS_SUCCESSFUL) || (rtEvtOut != expRtEvt))
    {
      ERROR_REPORT(SW_ERROR, rtStatus, rtEvtOut, expRtEvt);
      go = FALSE;
    }
    else
    {
      /* First cycle */
      pCtx->cycles = 0;
      pCtx->itInCycle = 0;
    }

    while (go)
    {
      /* update current Bus Cycle count */
      if (pCtx->itInCycle == 0)
      {
        pCtx->cycles++;
      }

      /* update current slots Cycle count */

      /* Update slots (Half Fast RTC counts) */
      if(pCtx->itInCycle%2 == 0)
      {
        pCtx->slots++;
      }

      /* store the number of elapsed it Slots in the Bus context */
      CanOpenBus_storeItFromStart(pCtx->busId, ((pCtx->cycles * FAST_RTC_SLOTS_IN_RTC) + pCtx->itInCycle));

      if ((pCtx->actPmHealth == E_CO_PM_HEALTH_SAFE) &&
          (pCtx->extCanHealth == E_CO_EXTCAN_HEALTH_SAFE))
      {
        pCtx->curState = E_COSTATE_OFF;
        CanOpenAction_updateHkArea(pCtx, E_COSTATE_OFF);
      }

      /* wait for the next IT sync event */
      slotToWait = (pCtx->itInCycle + 1) % FAST_RTC_SLOTS_IN_RTC;
      rtEvtOut = 0;
      expRtEvt = EVENT_FROM_BUS_SLOT(busId, slotToWait);

      rtStatus = rtems_event_receive (ALL_SYNC_EVENTS, RTEMS_WAIT|RTEMS_EVENT_ANY,
                                      RTEMS_NO_TIMEOUT, &rtEvtOut);

      rtems_task_suspend(RTEMS_SELF);
    }
  }
}

PRIVATE void cycleScetGet(U32 *pCoarseU32B, U16 *pFineU16B)
{
  U32 coarse = 0;
  U32 fine = 0;

  coarse = RtcCplr_syncObt.coarseOBT;
  fine = RtcCplr_syncObt.subSeconds;

  *pCoarseU32B = coarse;
  *pFineU16B = fine & U16_MSK;
}

PRIVATE Bool busMgrRestart(void)
{
  Uint bus;
  CoCtx *pCtx;
  rtems_status_code rtStatus;
  rtems_event_set rtEvt;
  Bool go = TRUE;

  /* send the event to the Bus managers */
  for (bus = 0; ((bus < ROV_CANBUS_NUM) && go); bus++)
  {
    pCtx = &CoMgr_ctx[bus];

    /* transmit cycle SCET time at the beginning of a cycle */
    if (rtcSyncCtx.itCurSlot == 0)
    {
      pCtx->cucC32B = rtcSyncCtx.coarseTime32B;
      pCtx->cucF16B = rtcSyncCtx.fineTime16B;
    }

    /* Update the Fast RTC slot and Half RTC slot */
    pCtx->itInCycle = rtcSyncCtx.itCurSlot;
    pCtx->slotInCycle = rtcSyncCtx.curSlot;

    /* Notify both CAN Buses of a new RTC or Fast RTC slot.
     * Use a different rtems event for each Fast RTC slot */
    rtEvt = EVENT_FROM_BUS_SLOT(bus, rtcSyncCtx.itCurSlot);
    rtStatus = rtems_event_send (pCtx->taskId, rtEvt);
    if (rtStatus != RTEMS_SUCCESSFUL)
    {
      printf("busMgrRestart: Failed to send event 0x%08X to Bus %u (rtStatus=%u)\n",
             rtEvt, bus, rtStatus);
      ERROR_REPORT(SW_ERROR, rtStatus, pCtx->taskId, rtEvt);
      go = FALSE;
    }
  }
  return go;
}

