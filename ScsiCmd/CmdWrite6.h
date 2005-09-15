#ifndef __CmdWrite6_h
#define __CmdWrite6_h


#include "common.h"


VECTOR
CmdWrite6(SCSI_HANDLE handle, COMMON_PARAMS common,
          VECTOR dat, int fixed, int length);   /* size, timeout */


#endif /* __CmdWrite6_h */
