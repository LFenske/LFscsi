#ifndef __CmdModeSense_h
#define __CmdModeSense_h


#include "common.h"


VECTOR
CmdModeSense(SCSI_HANDLE handle, COMMON_PARAMS common,
            int param);   /* size, timeout */


#endif /* __CmdModeSense_h */
