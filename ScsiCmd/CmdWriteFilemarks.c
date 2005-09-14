

#ifdef ENUM
  CMD_WriteFilemarks,
#endif


#ifdef DEF
  {CMD_WriteFilemarks, "write_filemarks", LineWriteFilemarks, DIRECTION_NONE, NULL, "", ""},
  {CMD_WriteFilemarks, "wf"             , LineWriteFilemarks, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdWriteFilemarks.h"

int
LineWriteFilemarks(SCSI_HANDLE handle, COMMON_PARAMS common,
                   int argc, char**argv)
{
  int count = 1;

  if (argc > 0) {
    count = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdWriteFilemarks(handle, common,
                            count);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdWriteFilemarks_c
#include "CmdWriteFilemarks.h"
#undef  __CmdWriteFilemarks_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdWriteFilemarks(SCSI_HANDLE handle, COMMON_PARAMS common,
                  int count)   /* immed, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x10;
  cdb[1] = common->immed ? 1 : 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = 0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   120.);
  return retval;
}
#endif


