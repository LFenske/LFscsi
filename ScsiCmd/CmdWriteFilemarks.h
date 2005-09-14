#ifndef __CmdWriteFilemarks_h
#define __CmdWriteFilemarks_h


#include "common.h"


VECTOR
CmdWriteFilemarks(SCSI_HANDLE handle, COMMON_PARAMS common,
                  int count);   /* immed, timeout */


#endif /* __CmdWriteFilemarks_h */
