#ifndef __CmdRead6_h
#define __CmdRead6_h


#include "common.h"


VECTOR
CmdRead6(SCSI_HANDLE handle, COMMON_PARAMS common,
         int sili_fixed, int length);   /* timeout */


#endif /* __CmdRead6_h */
