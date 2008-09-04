#ifndef __CmdReportLuns_h
#define __CmdReportLuns_h


#include "common.h"


VECTOR
CmdReportLuns(SCSI_HANDLE handle, COMMON_PARAMS common,
	      int selectreport);   /* size, timeout */


#endif /* __CmdReportLuns_h */
