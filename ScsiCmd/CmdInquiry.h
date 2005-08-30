#ifndef __CmdInquiry_h
#define __CmdInquiry_h


#include "common.h"


vector
CmdInquiry(SCSI_HANDLE device, bool evpd, int page_code);   /* size, timeout */


#endif /* __CmdInquiry_h */
