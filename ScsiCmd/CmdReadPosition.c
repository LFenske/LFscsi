

#ifdef ENUM
  CMD_ReadPosition,
#endif


#ifdef DEF
  {CMD_ReadPosition, "read_position", LineReadPosition, DIRECTION_IN, PrintReadPositionSub, "", ""},
  {CMD_ReadPosition, "rp"           , LineReadPosition, DIRECTION_IN, PrintReadPositionSub, "- alias for read_position", NULL},
#endif


#ifdef LINE
#include "CmdReadPosition.h"
#include "PrintReadPositionSub.h"

int
LineReadPosition(SCSI_HANDLE handle, COMMON_PARAMS common,
                 int argc, char**argv)
{
  if (argc > 0) {
    help(common);
    return -1;
  }

  {
    dat = CmdReadPosition(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdReadPosition_c
#include "CmdReadPosition.h"
#undef  __CmdReadPosition_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdReadPosition(SCSI_HANDLE handle, COMMON_PARAMS common)   /* timeout */
{
  byte cdb[10];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(20);
  retval.len = 20;

  cdb[0] = 0x34;
  cdb[1] = 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = 0;
  cdb[5] = 0;
  cdb[6] = 0;
  cdb[7] = 0;
  cdb[8] = 0;
  cdb[9] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   1.);
  return retval;
}
#endif


