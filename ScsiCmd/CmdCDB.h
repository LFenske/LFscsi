#ifndef __CmdCDB_h
#define __CmdCDB_h


#include "common.h"


VECTOR
CmdCDB(SCSI_HANDLE handle, COMMON_PARAMS common,
            int param);   /* size, timeout */


#endif /* __CmdCDB_h */
