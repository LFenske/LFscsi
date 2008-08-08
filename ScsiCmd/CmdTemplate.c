

#ifdef ENUM
  CMD_Template,
#endif


#ifdef DEF
  {CMD_Template, "template", LineTemplate, DIRECTION_TEMPLATE, PrintTemplateSub, "", ""},
#endif


#ifdef LINE
#include "CmdTemplate.h"

int
LineTemplate(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int param = -1;

  if (argc > optind) {
    param = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdTemplate(handle, common,
                      param);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdTemplate_c
#include "CmdTemplate.h"
#undef  __CmdTemplate_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdTemplate(SCSI_HANDLE handle, COMMON_PARAMS common,
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


