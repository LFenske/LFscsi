

#ifdef ENUM
  CMD_Reset,
#endif


#ifdef DEF
  {CMD_Reset, "reset"      , LineReset      , DIRECTION_NONE, NULL, "", ""},
  {CMD_Reset, "resetdevice", LineResetDevice, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdReset.h"

int
LineReset(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdReset(handle, common, RESET_BUS);
  }
  return 0;
}

int
LineResetDevice(SCSI_HANDLE handle, COMMON_PARAMS common,
                int argc, char**argv)
{
  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdReset(handle, common, RESET_DEVICE);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdReset_c
#include "CmdReset.h"
#undef  __CmdReset_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdReset(SCSI_HANDLE handle, COMMON_PARAMS common,
         RESET_LEVEL level)   /* */
{
  VECTOR retval;
  int status;
  retval.dat = NULL;
  retval.len = 0;
  common->stt.len = 0;
  status = (handle->reset)(handle, level);
  return retval;
}
#endif


