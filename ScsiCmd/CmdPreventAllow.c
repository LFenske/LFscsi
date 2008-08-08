

#ifdef ENUM
  CMD_PreventAllow,
#endif


#ifdef DEF
  {CMD_PreventAllow, "prevent", LinePrevent, DIRECTION_NONE, NULL, "", ""},
  {CMD_PreventAllow, "allow",   LineAllow  , DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdPreventAllow.h"

int
LinePrevent(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdPreventAllow(handle, common,
                          TRUE);
  }
  return 0;
}

int
LineAllow(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdPreventAllow(handle, common,
                          FALSE);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdPreventAllow_c
#include "CmdPreventAllow.h"
#undef  __CmdPreventAllow_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdPreventAllow(SCSI_HANDLE handle, COMMON_PARAMS common,
                bool prevent)   /* timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x1e;
  cdb[1] = 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = prevent ? 1 : 0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   1.);
  return retval;
}
#endif


