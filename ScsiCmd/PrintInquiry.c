#include "common.h"
#include "PrintInquirySub.h"
#include <stdlib.h>   /* for malloc */
#include <unistd.h>   /* for read, getopt */


int
main(int argc, char**argv)
{
  VECTOR dat;
  byte *dp;
  int left = 0x10000;
  int thislen;
  bool force_stdpage = FALSE;

  {
    int ch;
    while ((ch = getopt(argc, argv, "s")) != -1) {
      switch (ch) {
      case 's': force_stdpage = TRUE; break;
      case '?':
      default:
        {
          /*stub: usage(progname);*/
          exit(-1);
        }
        break;
      }
    }
  }

  dp = dat.dat = malloc(left);
  while ((thislen = read(0, dp, left))) {
    dp   += thislen;
    left -= thislen;
  }
  dat.len = dp - dat.dat;
  if (force_stdpage && dat.dat[2] == 0)
    dat.dat[2] = 0xff;
  PrintInquirySub(dat);
  free(dat.dat);
  return 0;
}


