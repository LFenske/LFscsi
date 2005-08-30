#define __CmdInquiry_c

#include "CmdInquiry.h"
#include "ScsiTransport.h"
#include <stdlib.h>   /* for malloc */


vector
CmdInquiry(SCSI_HANDLE device, bool evpd, int page_code)   /* size, timeout */
{
  byte cdb[6];
  int thissize = (size != NOSIZE) ? size : 0xff;
  vector retval;
  retval.dat = malloc(thissize);
  retval.len = thissize;
  cdb[0] = 0x12;
  cdb[1] = evpd;
  cdb[2] = page_code;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(device,
	   DIRECTION_IN,
	   cdb, sizeof(cdb),
	   retval,
	   5.);
  return retval;
}
