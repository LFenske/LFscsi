

#ifdef ENUM
CMD_ModeSense,
#endif


#ifdef DEF
{CMD_ModeSense, "mode_sense", LineModeSense, DIRECTION_IN, PrintModeSenseSub6, "[-c<cdb size>] [-p<page control>] [<page code> [<subpage code>]]", "defaults:\n  cdb size = 6\n  page control = 0 (current values)\n  page code = 0x3f (all pages)\n  subpage code = 0\n"},
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
  int cdb_size     = 6;
  int page_control = 0;
  int page_code    = 0x3f;
  int subpage_code = 0;

  int ch;
  /*optreset = 1;*/
  optind = 0;
  while ((ch = getopt(argc, argv, "c:p:")) != -1) {
    switch (ch) {
    case 'c':
      cdb_size     = strtol(optarg, (char**)NULL, 0);
      break;
    case 'p':
      page_control = strtol(optarg, (char**)NULL, 0);
      break;
    case '?':
    default:
      help(common);
      break;
    }
  }
  argc -= optind;
  argv += optind;

  if (argc > 0) {
    page_code = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }
  if (argc > 0) {
    subpage_code = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  if (argc > 0) {
    help(common);
    return -1;
  }

  {
    dat = CmdModeSense(handle, common,
                       cdb_size, page_control, page_code, subpage_code);
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
#include <stdlib.h>   /* for malloc */

VECTOR
CmdModeSense(SCSI_HANDLE handle, COMMON_PARAMS common,
             int cdb_size,
             int page_control,
             int page_code,
             int subpage_code)   /* size, timeout */
{
  byte cdb[10];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->size != NOSIZE) ? common->size : (cdb_size == 6) ? 0xff : 0x1000;

  int DBD   = 0;
  int LLBAA = 0;

  cdbvec.dat = cdb;
  cdbvec.len = cdb_size;
  retval.dat = malloc(thissize);
  retval.len = thissize;

  switch (cdb_size) {
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


