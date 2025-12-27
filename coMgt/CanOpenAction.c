#include "coMgt/CanOpenAction.h"
#include "coMgt/CanOpenBus.h"

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
}

PUBLIC void CanOpenAction_updateHkArea(CoCtx *pCtx, CoState entryState)
{

}
