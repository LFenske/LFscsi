

#ifdef ENUM
  CMD_LogSelect,
#endif


#ifdef DEF
  {CMD_LogSelect, "template", LineLogSelect, DIRECTION_TEMPLATE, PrintLogSelectSub, "", ""},
#endif


#ifdef LINE
#include "CmdLogSelect.h"

int
LineLogSelect(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int param = -1;

  if (argc > 0) {
    param = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdLogSelect(handle, common,
                      param);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdLogSelect_c
#include "CmdLogSelect.h"
#undef  __CmdLogSelect_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdLogSelect(SCSI_HANDLE handle, COMMON_PARAMS common,
           bool evpd, int page_code)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->size != NOSIZE) ? common->size : 0xff;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x??;
  cdb[1] = ;
  cdb[2] = ;
  cdb[3] = ;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_TEMPLATE,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif

