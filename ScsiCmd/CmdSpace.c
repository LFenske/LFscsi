

#ifdef ENUM
  CMD_Space,
#endif


#ifdef DEF
  {CMD_Space, "space", LineSpace, DIRECTION_NONE, NULL, "<code> <count>", "'code' can be b,B,r,R for blocks, f,F for filemarks, s,S for sequential filemarks, e,E for end of data, or a number\nDo not use 'count' field for EOD"},
#endif


#ifdef LINE
#include "CmdSpace.h"

int
LineSpace(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  int code  = 0;
  int count = 0;

  if (argc < 1) {
    /*stub: usage(progname);*/
    return -1;
  }

  if (argc > 0) {
    switch (argv[0][0]) {
    case 'b':
    case 'B':
    case 'r':
    case 'R':
      code = 0;
      break;
    case 'f':
    case 'F':
      code = 1;
      break;
    case 's':
    case 'S':
      code = 2;
      break;
    case 'e':
    case 'E':
      code = 3;
      break;
    default:
      code = strtol(argv[0], (char**)NULL, 0);
      break;
    }
    argc--;
    argv++;
  }
  if (code != 3) {
    if (argc < 1) {
      /*stub: usage(progname);*/
      return -1;
    } else {
      count = strtol(argv[0], (char**)NULL, 0);
    }
    argc--;
    argv++;
  }

  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdSpace(handle, common,
                   code, count);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdSpace_c
#include "CmdSpace.h"
#undef  __CmdSpace_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdSpace(SCSI_HANDLE handle, COMMON_PARAMS common,
         int code, int count)   /* timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x11;
  cdb[1] = code & 0x0f;
  cdb[2] = count >> 16;
  cdb[3] = count >>  8;
  cdb[4] = count >>  0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   30.);
  return retval;
}
#endif

