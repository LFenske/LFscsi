

#ifdef ENUM
CMD_ModeSense,
#endif


#ifdef DEF
{CMD_ModeSense, "mode_sense", LineModeSense, DIRECTION_IN, PrintModeSenseSub6, "[-p<page control>] [-b] [<page code> [<subpage code>]]", "defaults:\n  page control = 0 (current values)\n  page code = 0x3f (all pages)\n  subpage code = 0\n"},
{CMD_ModeSense, "sense"     , LineModeSense, DIRECTION_IN, PrintModeSenseSub6, "- alias for mode_sense", NULL},
#endif


#ifdef LINE
#include "CmdModeSense.h"
#include "PrintModeSenseSub.h"
#include <unistd.h>   /* for getopt */

extern char *optarg;
extern int   optind;
extern int   optreset;

int
LineModeSense(SCSI_HANDLE handle, COMMON_PARAMS common,
              int argc, char**argv)
{
  int DBD          = 0;
  int page_control = 0;
  int page_code    = 0x3f;
  int subpage_code = 0;

  int ch;
  /*optreset = 1;*/
  while ((ch = getopt(argc, argv, "bp:")) != -1) {
    switch (ch) {
    case 'b':
      DBD = 1;
      break;
    case 'p':
      page_control = strtol(optarg, (char**)NULL, 0);
      break;
    case '?':
    default:
      //help(common);
      break;
    }
  }

  if (argc > optind) {
    page_code = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }
  if (argc > optind) {
    subpage_code = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdModeSense(handle, common,
                       DBD, page_control, page_code, subpage_code);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdModeSense_c
#include "CmdModeSense.h"
#undef  __CmdModeSense_c

#include "ScsiTransport.h"
#include "common.h"
#include "PrintModeSenseSub.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdModeSense(SCSI_HANDLE handle, COMMON_PARAMS common,
	     int DBD,
             int page_control,
             int page_code,
             int subpage_code)   /* size, timeout */
{
  byte cdb[10];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize;

  int LLBAA = 0;

  if (common->cdb_size == NOSIZE)
    common->cdb_size = 6;
  thissize = (common->dat_size != NOSIZE) ? common->dat_size : (common->cdb_size == 6) ? 0xff : 0x1000;
  cdbvec.dat = cdb;
  cdbvec.len = common->cdb_size;
  retval.dat = malloc(thissize);
  retval.len = thissize;

  switch (common->cdb_size) {
  case 6:
    cdb[0] = 0x1a;
    cdb[1] =
      ((DBD   & 1) << 3) |
      0;
    cdb[2] =
      (page_control & 0x03) << 6 |
      (page_code    & 0x3f) << 0 |
      0;
    cdb[3] = subpage_code;
    cdb[4] = thissize;
    cdb[5] = 0;
    common->printer = PrintModeSenseSub6;
    break;
  case 10:
    cdb[0] = 0x5a;
    cdb[1] =
      ((LLBAA & 1) << 4) |
      ((DBD   & 1) << 3) |
      0;
    cdb[2] =
      ((page_control & 0x03) << 6) |
      ((page_code    & 0x3f) << 0) |
      0;
    cdb[3] = subpage_code;
    cdb[4] = 0;
    cdb[5] = 0;
    cdb[6] = 0;
    cdb[7] = thissize >> 8;
    cdb[8] = thissize >> 0;
    cdb[9] = 0;
    common->printer = PrintModeSenseSub10;
    break;
  default:
    /*stub: error */
    break;
  }
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif


