

#ifdef ENUM
  CMD_ReportLuns,
#endif


#ifdef DEF
  {CMD_ReportLuns, "reportluns", LineReportLuns, DIRECTION_IN, PrintDefaultSub, "[<select report>]", "defaults to 2"},
#endif


#ifdef LINE
#include "CmdReportLuns.h"

int
LineReportLuns(SCSI_HANDLE handle, COMMON_PARAMS common,
	       int argc, char**argv)
{
  int selectreport = 2;

  if (argc > optind) {
    selectreport = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdReportLuns(handle, common,
			selectreport);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdReportLuns_c
#include "CmdReportLuns.h"
#undef  __CmdReportLuns_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdReportLuns(SCSI_HANDLE handle, COMMON_PARAMS common,
	      int selectreport)   /* size, timeout */
{
  byte cdb[12];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 0x10;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[ 0] = 0xa0;
  cdb[ 1] = 0;
  cdb[ 2] = selectreport;
  cdb[ 3] = 0;
  cdb[ 4] = 0;
  cdb[ 5] = 0;
  cdb[ 6] = thissize >> 24;
  cdb[ 7] = thissize >> 16;
  cdb[ 8] = thissize >>  8;
  cdb[ 9] = thissize >>  0;
  cdb[10] = 0;
  cdb[11] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif


