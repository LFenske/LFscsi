#ifndef __CmdTemplate_h
#define __CmdTemplate_h


#include "common.h"


VECTOR
CmdTemplate(SCSI_HANDLE handle, COMMON_PARAMS common,
            int param);   /* size, timeout */


#endif /* __CmdTemplate_h */
