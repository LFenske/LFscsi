#ifndef __CmdModeSelect_h
#define __CmdModeSelect_h


#include "common.h"


VECTOR
CmdModeSelect(SCSI_HANDLE handle, COMMON_PARAMS common,
              int cdb_size, VECTOR dat, bool page_format);   /* save, size, timeout */


#endif /* __CmdModeSelect_h */
