#define __CmdInquiry_c

#include "CmdInquiry.h"
#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */


VECTOR
CmdInquiry(SCSI_HANDLE handle, COMMON_PARAMS common,
           bool evpd, int page_code)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->size != NOSIZE) ? common->size : 0xff;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x12;
  cdb[1] = evpd;
  cdb[2] = page_code;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
