#ifndef __CmdLoadUnload_h
#define __CmdLoadUnload_h


#include "common.h"


VECTOR
CmdLoadUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
              int toLoad);   /* immed, timeout */


#endif /* __CmdLoadUnload_h */
