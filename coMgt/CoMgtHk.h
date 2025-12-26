#ifndef _CoMgtHk_
#define _CoMgtHk_

#define CO_STATUS_FAIL (0)
#define CO_STATUS_SAFE (1)

typedef enum
{
  E_CO_PM_HEALTH_FAIL = CO_STATUS_FAIL,
  E_CO_PM_HEALTH_SAFE = CO_STATUS_SAFE
} CoPmHealthSts;

typedef enum
{
  E_CO_EXTCAN_HEALTH_FAIL = CO_STATUS_FAIL,
  E_CO_EXTCAN_HEALTH_SAFE = CO_STATUS_SAFE
} CoExtCanHealthSts;
#endif /* _CoMgtHk_ */