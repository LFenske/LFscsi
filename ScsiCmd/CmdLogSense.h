#ifndef __CmdLogSense_h
#define __CmdLogSense_h


#include "common.h"


VECTOR
CmdLogSense(SCSI_HANDLE handle, COMMON_PARAMS common,
            int param);   /* size, timeout */


#endif /* __CmdLogSense_h */
