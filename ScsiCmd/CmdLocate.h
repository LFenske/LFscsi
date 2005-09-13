#ifndef __CmdLocate_h
#define __CmdLocate_h


#include "common.h"


VECTOR
CmdLocate(SCSI_HANDLE handle, COMMON_PARAMS common,
          int objid);   /* immed, timeout */


#endif /* __CmdLocate_h */
