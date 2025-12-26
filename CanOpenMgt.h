#ifndef _CanOpenMgt_
#define _CanOpenMgt_
#include "../Stub.h"

PUBLIC void CanOpenMgt_init(U16 evtPid);
PUBLIC void CanOpenMgt_syncTaskBody(rtems_task_argument unused);
PUBLIC void CanOpenMgt_busMgr(rtems_task_argument busId);

#endif /* _CanOpenMgt_ */