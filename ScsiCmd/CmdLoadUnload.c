

#ifdef ENUM
  CMD_LoadUnload,
#endif


#ifdef DEF
  {CMD_LoadUnload, "load"  , LineLoad  , DIRECTION_NONE, NULL, "", ""},
  {CMD_LoadUnload, "unload", LineUnload, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdLoadUnload.h"

int
LineLoadUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
               int argc, char**argv,
               int toLoad)
{
  if (argc > 0) {
    toLoad = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdLoadUnload(handle, common,
                        toLoad);
  }
  return 0;
}


int
LineLoad(SCSI_HANDLE handle, COMMON_PARAMS common,
         int argc, char**argv)
{
  return LineLoadUnload(handle, common, argc, argv, 1);
}


int
LineUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
           int argc, char**argv)
{
  return LineLoadUnload(handle, common, argc, argv, 0);
}


#endif


#ifdef COMMAND
#define __CmdLoadUnload_c
#include "CmdLoadUnload.h"
#undef  __CmdLoadUnload_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdLoadUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
              int toLoad)   /* immed, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x1b;
  cdb[1] = common->immed ? 1 : 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = toLoad;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   120.);
  return retval;
}
#endif


