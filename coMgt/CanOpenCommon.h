#ifndef _CanOpenCommon_
#define _CanOpenCommon_

/** Platform Can Bus ID */
#define CANBUS_ID_PF (0)

/** Payload CAN Bus ID */
#define CANBUS_ID_PL (1)

/** There are 2 CAN Buses */
#define ROV_CANBUS_NUM (2)

typedef enum
{
  E_COSTATE_OFF = 0,
  E_COSTATE_INIT = 1,
  E_COSTATE_PRE_OP = 2,
  E_COSTATE_OP = 3,
  E_COSTATE_STOP = 4,
  E_COSTATE_END
} CoState;

typedef enum
{
  E_COSTATUS_OK = 0,
  E_COSTATUS_ERROR = 0x100,
  E_COSTATUS_PARAM_ERROR,
  E_COSTATUS_MSG_LIST_FULL,
  E_COSTATUS_CAN_IF_ERROR,
  E_COSTATUS_EXTCAN_ERROR,
  E_COSTATUS_ACTPM_ERROR,
  E_COSTATUS_BUS_BUSY
} CoStatus;

#endif /* _CanOpenCOmmon_ */