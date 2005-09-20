#ifndef __CmdPreventAllow_h
#define __CmdPreventAllow_h


#include "common.h"


VECTOR
CmdPreventAllow(SCSI_HANDLE handle, COMMON_PARAMS common,
                bool prevent);   /* timeout */


#endif /* __CmdPreventAllow_h */
