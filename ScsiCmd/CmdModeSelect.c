

#ifdef ENUM
  CMD_ModeSelect,
#endif


#ifdef DEF
  {CMD_ModeSelect, "mode_select"  , LineModeSelect, DIRECTION_OUT, NULL, "<page format> [<data>]", ""},
  {CMD_ModeSelect, "select"       , LineModeSelect, DIRECTION_OUT, NULL, "- alias for mode_select", NULL},
  {CMD_ModeSelect, "setblkbuf"    , LineSetBlkBuf , DIRECTION_OUT, NULL, "<block size> <buffer mode>", ""},
  {CMD_ModeSelect, "setctltimeout", LineSetCtlTO  , DIRECTION_OUT, NULL, "<ctl_time_io_secs>", ""},
#endif


#ifdef LINE
#include "CmdModeSelect.h"

int
LineModeSelect(SCSI_HANDLE handle, COMMON_PARAMS common,
               int argc, char**argv)
{
  int cdb_size     = 6;
  bool page_format = TRUE;

#if 0
  int ch;
  /*optreset = 1;*/
  optind = 0;
  while ((ch = getopt(argc, argv, "c:")) != -1) {
    switch (ch) {
    case 'c':
      cdb_size     = strtol(optarg, (char**)NULL, 0);
      break;
    case '?':
    default:
      help(common);
      break;
    }
  }
  argc -= optind;
  argv += optind;
#endif

  if (argc < optind+1) {
    help(common);
    return -1;
  }

  page_format = strtol(argv[optind++], (char**)NULL, 0) != 0;

  {
    /*stub: fill in the data */
    if (argc > optind) {
      int i;
      dat.len = argc-optind;
      dat.dat = malloc(dat.len);
      for (i=0; i<argc-optind; i++) {
        dat.dat[i] = strtol(argv[i+optind], (char**)NULL, 0);
      }
      common->dat_size = dat.len;
    } else {
      int bytesgotten = 0;
      dat.len = (common->dat_size != NOSIZE) ? common->dat_size : 12;
      dat.dat = malloc(dat.len);
      while (bytesgotten < dat.len) {
        int bytesthistime = read(0, dat.dat+bytesgotten, dat.len-bytesgotten);
        if (bytesthistime == 0) {
          fprintf(stderr, "not enough data for mode select, got %d, expected %d\n", bytesgotten, dat.len);
          free(dat.dat);
          return(-2);
        }
        bytesgotten += bytesthistime;
      }
    }

    CmdModeSelect(handle, common,
                  cdb_size, dat, page_format);
  }
  return 0;
}


int
LineSetBlkBuf(SCSI_HANDLE handle, COMMON_PARAMS common,
              int argc, char**argv)
{
  int blocksize = 0;
  int buffermode = 1;

  if (argc > optind) { blocksize  = strtol(argv[optind], (char**)NULL, 0); optind++;}
  if (argc > optind) { buffermode = strtol(argv[optind], (char**)NULL, 0); optind++;}

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat.len = 12;
    dat.dat = malloc(dat.len);
    common->dat_size = dat.len;
    dat.dat[ 0] = 0;
    dat.dat[ 1] = 0;
    dat.dat[ 2] = (buffermode&7) << 4;
    dat.dat[ 3] = 8;
    dat.dat[4+0] = 0;
    dat.dat[4+1] = 0;
    dat.dat[4+2] = 0;
    dat.dat[4+3] = 0;
    dat.dat[4+4] = 0;
    dat.dat[4+5] = blocksize >> 16;
    dat.dat[4+6] = blocksize >>  8;
    dat.dat[4+7] = blocksize >>  0;

    CmdModeSelect(handle, common,
                  6, dat, TRUE);
  }
  return 0;
}


int
LineSetCtlTO(SCSI_HANDLE handle, COMMON_PARAMS common,
	     int argc, char**argv)
{
  int ctl_time_io_secs = 90;

  if (argc > optind) { ctl_time_io_secs = strtol(argv[optind], (char**)NULL, 0); optind++;}

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat.len = 12;
    dat.dat = malloc(dat.len);
    common->dat_size = dat.len;
    dat.dat[ 0] = 11;      /* length after this */
    dat.dat[ 1] = 0;
    dat.dat[ 2] = 0;
    dat.dat[ 3] = 0;
    dat.dat[4+0] = 0x40;   /* page_code */
    dat.dat[4+1] = 0xf0;   /* subpage */
    dat.dat[4+2] = 4 >> 8; /* page_length */
    dat.dat[4+3] = 4 >> 0; /* page_length */
    dat.dat[4+4] = 0x00;   /* page_version */
    dat.dat[4+5] = ctl_time_io_secs >> 8;
    dat.dat[4+6] = ctl_time_io_secs >> 0;
    dat.dat[4+7] = 0;      /* fill */

    CmdModeSelect(handle, common,
                  6, dat, TRUE);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdModeSelect_c
#include "CmdModeSelect.h"
#undef  __CmdModeSelect_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdModeSelect(SCSI_HANDLE handle, COMMON_PARAMS common,
              int cdb_size, VECTOR dat, bool page_format)   /* save, size, timeout */
{
  byte cdb[10];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 0xff;

  cdbvec.dat = cdb;
  cdbvec.len = cdb_size;
  retval.dat = NULL;
  retval.len = 0;

  switch (cdb_size) {
  case 6:
     cdb[0] = 0x15;
     cdb[1] = (page_format ? 1 : 0) << 4;
     cdb[2] = 0;
     cdb[3] = 0;
     cdb[4] = thissize;
     cdb[5] = 0;
     break;
  case 10:
     cdb[0] = 0x55;
     cdb[1] = (page_format ? 1 : 0) << 4;
     cdb[2] = 0;
     cdb[3] = 0;
     cdb[4] = 0;
     cdb[5] = 0;
     cdb[6] = 0;
     cdb[7] = thissize >> 8;
     cdb[8] = thissize >> 0;
     cdb[9] = 0;
     break;
  default:
     break;
  }
  send_cdb(handle, common,
           DIRECTION_OUT,
           cdbvec,
	   dat,
	   1.);
  return retval;
}
#endif


