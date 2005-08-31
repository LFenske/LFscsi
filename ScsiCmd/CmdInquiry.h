#ifndef __CmdInquiry_h
#define __CmdInquiry_h


#include "common.h"


VECTOR
CmdInquiry(SCSI_HANDLE handle, COMMON_PARAMS common,
           bool evpd, int page_code);   /* size, timeout */


#endif /* __CmdInquiry_h */
