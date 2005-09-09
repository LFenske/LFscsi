

#ifdef ENUM
  CMD_INQUIRY,
#endif


#ifdef DEF
  {CMD_INQUIRY, "inquiry", LineInquiry, DIRECTION_IN , "PrintInquiry", "[-z size] [-r] [page code]", "page code"},
  {CMD_INQUIRY, "inq"    , LineInquiry, DIRECTION_IN , "PrintInquiry", "[-z size] [-r] [page code]", "page code"},
#endif


#ifdef LINE
#include "CmdInquiry.h"

int
LineInquiry(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int page = -1;

  if (argc > 0) {
    page = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  if (argc > 0) {
    /*stub: usage(progname);*/
    return -1;
  }

  {
    dat = CmdInquiry(handle, common,
                     (page == -1) ? 0 : 1,
                     (page == -1) ? 0 : page
                     );
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdInquiry_c
#include "CmdInquiry.h"
#undef  __CmdInquiry_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdInquiry(SCSI_HANDLE handle, COMMON_PARAMS common,
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

  cdb[0] = 0x12;
  cdb[1] = evpd;
  cdb[2] = page_code;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif


