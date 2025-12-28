#include "coMgt/CanOpenAction.h"
#include "coMgt/CanOpenBus.h"

typedef struct CoTrans
{
    CoState  curState; /**< @brief current automaton state */
    CoEvt    evtIn;    /**< @brief input event */
    CoAction action;   /**< @brief corresponding 'action' to be performed */
} CoTrans;

PRIVATE CoTrans const *coTransGet(CoEvt, CoState state);
PRIVATE CoEvt coEvtDecode (CoCtx *pCtx);

CoCtx CoMgr_ctx[ROV_CANBUS_NUM] =
{
  {
    .busName = "PLAT",
    .busId = CANBUS_ID_PF
  },
  {
    .busName = "PAYL",
    .busId = CANBUS_ID_PL
  }
};

PUBLIC CoStatus CanOpenAction_setup(CoCtx *pCtx, U16 evtPid)
{
  return E_COSTATUS_OK;
}

void CanOpenAction_busMgrAutom(CoCtx *pCtx)
{
  CoEvt evt = E_COEVT_END;
  CoState state = E_COSTATE_END;
  CoTrans const *pTrans = NULL;

  while (pTrans == NULL)
  {
    /* retrieve the next event */
    evt = coEvtDecode (pCtx);
  
    /* identify the action */
    pTrans = coTransGet(evt, pCtx->curState);
  }

  state = pTrans->action(pCtx);
  pCtx->lastEvt = evt;
  pCtx->nEvt++;
}

PUBLIC void CanOpenAction_updateHkArea(CoCtx *pCtx, CoState entryState)
{

}

PRIVATE CoTrans const *coTransGet(CoEvt, CoState state)
{
  return 0;
}

PRIVATE CoEvt coEvtDecode (CoCtx *pCtx)
{
  CoEvt coEvt;
  Uint idx;
  Uint cmd;
  
  #if 0
  /* upon CYCLE event */
  if ((pCtx->slotInCycle == 0) && (pCtx->itInCycle == 0))
  {
    /* upon a Cycle event, get input mstCmd if we are not in INIT */
    if ((pCtx->mstCmdIn > pCtx->mstCmdOut) && (pCtx->curState > E_COSTATE_INIT))
    {
      idx = pCtx->mstCmdOut%CO_MST_CMD_QUEUE_LENGTH;
      cmd = pCtx->mstCmdQueue[idx];
      pCtx->mstCmdOut++;
      coEvt = coEvtFromNmtCs(cmd);
    }
    /* check toggle timeout if we are in PRE_OP or OP mode */
    else if ((pCtx->ttoggleTimer < 0) && 
        ((pCtx->curState == E_COSTATE_PRE_OP)||(pCtx->curState == E_COSTATE_OP)))
    {
      coEvt = E_COEVT_TT_TIMEOUT;
    }
    /* check treset timeout if we are in OP mode */
    else if ((pCtx->tresetTimer < 0) && 
             ((pCtx->curState == E_COSTATE_PRE_OP)||(pCtx->curState == E_COSTATE_OP)))
    {
      if(pCtx->curState == E_COSTATE_OP)
      {
        pCtx->tresetTriggeredInOp = TRUE;
      }
      coEvt = E_COEVT_RESET;
    }
    else if((pCtx->tresetTriggeredInOp == TRUE) && 
            (pCtx->curState == E_COSTATE_PRE_OP))
    {
      pCtx->tresetTriggeredInOp = FALSE;
      coEvt = E_COEVT_START;
    }
    else /* otherwise, simple cycle event */
    {
      coEvt = E_COEVT_10HZ;
    }
  }
  /* upon slot event */
  else
  {
    coEvt = E_COEVT_200HZ;
  }
  
  #endif
  return coEvt;
}
