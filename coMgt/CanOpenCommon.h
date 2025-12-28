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

typedef enum
{
  E_COEVT_10HZ = 20,	/**< @brief 10Hz RTC event */
  E_COEVT_200HZ,	/**< @brief 200Hz Timer event */
  E_COEVT_START,	/**< @brief Start (NMT) command event */
  E_COEVT_STOP,		/**< @brief Stop (NMT) command event */
  E_COEVT_ENTER_PRE_OP, /**< @brief EntryPreOp (NMT) command event */
  E_COEVT_RESET,	/**< @brief Reset (NMT) command event */
  E_COEVT_SWITCH_BUS,	/**< @brief Switch Bus (TC) command event */
  E_COEVT_TT_TIMEOUT,	/**< @brief TToggle Timeout event */
  E_COEVT_IGNORED,	/**< @brief ignored event */
  E_COEVT_END		/**< @brief limit marker */
} CoEvt;

#endif /* _CanOpenCOmmon_ */
