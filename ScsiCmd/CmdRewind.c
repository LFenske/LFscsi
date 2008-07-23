

#ifdef ENUM
  CMD_Rewind,
#endif


#ifdef DEF
  {CMD_Rewind, "rewind", LineRewind, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdRewind.h"

int
LineRewind(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  if (argc > 0) {
    help(common);
    return -1;
  }

  {
    dat = CmdRewind(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdRewind_c
#include "CmdRewind.h"
#undef  __CmdRewind_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdRewind(SCSI_HANDLE handle, COMMON_PARAMS common)   /* immed, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x01;
  cdb[1] = common->immed ? 1 : 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = 0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   60.);
  return retval;
}
#endif


