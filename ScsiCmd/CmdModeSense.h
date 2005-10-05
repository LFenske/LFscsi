#ifndef __CmdModeSense_h
#define __CmdModeSense_h


#include "common.h"


VECTOR
CmdModeSense(SCSI_HANDLE handle, COMMON_PARAMS common,
             int cdb_size,
             int page_control,
             int page_code,
             int subpage_code);   /* size, timeout */


#endif /* __CmdModeSense_h */
