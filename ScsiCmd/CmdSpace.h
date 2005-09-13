#ifndef __CmdSpace_h
#define __CmdSpace_h


#include "common.h"


VECTOR
CmdSpace(SCSI_HANDLE handle, COMMON_PARAMS common,
         int code, int count);   /* timeout */


#endif /* __CmdSpace_h */
