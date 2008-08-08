

#ifdef ENUM
  CMD_Write6,
#endif


#ifdef DEF
  {CMD_Write6, "write6", LineWrite6, DIRECTION_OUT, NULL, "<fixed> <length>", ""},
#endif


#ifdef LINE
#include "CmdWrite6.h"

int
LineWrite6(SCSI_HANDLE handle, COMMON_PARAMS common,
           int argc, char**argv)
{
  int fixed;
  int length;

  if (argc < optind+2) {
    help(common);
    return -1;
  }

  fixed      = strtol(argv[optind], (char**)NULL, 0); optind++;
  length     = strtol(argv[optind], (char**)NULL, 0); optind++;

  {
    /*stub: fill in the data */
    if (argc > optind) {
      int i;
      dat.len = argc-optind;
      dat.dat = malloc(dat.len);
      for (i=optind; i<argc; i++) {
        dat.dat[i-optind] = strtol(argv[i], (char**)NULL, 0);
      }
    } else {
      int bytesgotten = 0;
      dat.len = (common->dat_size != NOSIZE) ? common->dat_size : length;
      dat.dat = malloc(dat.len);
      while (bytesgotten < dat.len) {
        int bytesthistime = read(0, dat.dat+bytesgotten, dat.len-bytesgotten);
        if (bytesthistime == 0) {
          fprintf(stderr, "not enough data for write, got %d, expected %d\n", bytesgotten, dat.len);
          free(dat.dat);
          return(-2);
        }
        bytesgotten += bytesthistime;
      }
    }

    CmdWrite6(handle, common,
              dat, fixed, length);

    free(dat.dat);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdWrite6_c
#include "CmdWrite6.h"
#undef  __CmdWrite6_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdWrite6(SCSI_HANDLE handle, COMMON_PARAMS common,
          VECTOR dat, int fixed, int length)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x0a;
  cdb[1] = fixed;
  cdb[2] = length >> 16;
  cdb[3] = length >>  8;
  cdb[4] = length >>  0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_OUT,
	   cdbvec,
	   dat,
	   5.);
  return retval;
}
#endif


