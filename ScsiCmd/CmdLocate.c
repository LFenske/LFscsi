

#ifdef ENUM
  CMD_Locate,
#endif


#ifdef DEF
  {CMD_Locate, "locate", LineLocate, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdLocate.h"

int
LineLocate(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int objid;

  if (argc != 1) {
    /*stub: usage(progname);*/
    return -1;
  }

  if (argc > 0) {
    objid = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  {
    dat = CmdLocate(handle, common,
                    objid);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdLocate_c
#include "CmdLocate.h"
#undef  __CmdLocate_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdLocate(SCSI_HANDLE handle, COMMON_PARAMS common,
          int objid)   /* immed, timeout */
{
  byte cdb[10];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x2b;
  cdb[1] = common->immed ? 1 : 0;  /* BT and CP are always 0 */
  cdb[2] = 0;
  cdb[3] = objid >> 24;
  cdb[4] = objid >> 16;
  cdb[5] = objid >>  8;
  cdb[6] = objid >>  0;
  cdb[7] = 0;
  cdb[8] = 0;
  cdb[9] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   120.);
  return retval;
}
#endif


