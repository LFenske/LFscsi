

#ifdef ENUM
  CMD_RequestSense,
#endif


#ifdef DEF
  {CMD_RequestSense, "request_sense", LineRequestSense, DIRECTION_IN, PrintRequestSenseSub, "", ""},
  {CMD_RequestSense, "rs"           , LineRequestSense, DIRECTION_IN, PrintRequestSenseSub, "", ""},
#endif


#ifdef LINE
#include "CmdRequestSense.h"
#include "PrintRequestSenseSub.h"

int
LineRequestSense(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdRequestSense(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdRequestSense_c
#include "CmdRequestSense.h"
#undef  __CmdRequestSense_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdRequestSense(SCSI_HANDLE handle, COMMON_PARAMS common)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->size != NOSIZE) ? common->size : 18;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x03;
  cdb[1] = 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   1.);
  return retval;
}
#endif


