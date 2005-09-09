

#ifdef ENUM
  CMD_TestUnitReady,
#endif


#ifdef DEF
  {CMD_TestUnitReady, "testunitready", LineTestUnitReady, DIRECTION_NONE, NULL, "", ""},
  {CMD_TestUnitReady, "tur"          , LineTestUnitReady, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdTestUnitReady.h"

int
LineTestUnitReady(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdTestUnitReady(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdTestUnitReady_c
#include "CmdTestUnitReady.h"
#undef  __CmdTestUnitReady_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdTestUnitReady(SCSI_HANDLE handle, COMMON_PARAMS common)   /* timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x00;
  cdb[1] = 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = 0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   1.);
  return retval;
}
#endif


