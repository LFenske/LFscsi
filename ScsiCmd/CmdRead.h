#ifndef __CmdRead_h
#define __CmdRead_h


#include "common.h"


VECTOR
CmdRead(SCSI_HANDLE handle, COMMON_PARAMS common,
	long long param1, long length);   /* size, timeout */


#endif /* __CmdRead_h */
