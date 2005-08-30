#define __CmdInquiry_c

#include "CmdInquiry.h"
#include "ScsiTransport.h"
#include <stdlib.h>   /* for malloc */


VECTOR
CmdInquiry(SCSI_HANDLE device, COMMON_PARAMS common,
           bool evpd, int page_code)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  cdbvec.dat = &cdb;
  cdbvec.len = sizeof(cdb);
  int thissize = (common->size != NOSIZE) ? common->size : 0xff;
  VECTOR retval;
  retval.dat = malloc(thissize);
  retval.len = thissize;
  cdb[0] = 0x12;
  cdb[1] = evpd;
  cdb[2] = page_code;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(device, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
