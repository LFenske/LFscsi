

#ifdef ENUM
  CMD_Read6,
#endif


#ifdef DEF
  {CMD_Read6, "read6", LineRead6, DIRECTION_IN, PrintDefaultSub, "<SILI-fixed> <length>", ""},
#endif


#ifdef LINE
#include "CmdRead6.h"
#include "PrintDefaultSub.h"

int
LineRead6(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  int sili_fixed;
  int length;

  if (argc != optind+2) {
    help(common);
    return -1;
  }

  sili_fixed = strtol(argv[optind], (char**)NULL, 0); optind++;
  length     = strtol(argv[optind], (char**)NULL, 0); optind++;

  {
    dat = CmdRead6(handle, common,
                   sili_fixed, length);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdRead6_c
#include "CmdRead6.h"
#undef  __CmdRead6_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdRead6(SCSI_HANDLE handle, COMMON_PARAMS common,
         int sili_fixed, int length)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : length;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x08;
  cdb[1] = sili_fixed;
  cdb[2] = length >> 16;
  cdb[3] = length >>  8;
  cdb[4] = length >>  0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   30.);
  return retval;
}
#endif


