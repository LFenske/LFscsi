#include "common.h"
#include "PrintModeSenseSub.h"
#include <stdlib.h>   /* for malloc */
#include <unistd.h>   /* for read, getopt */


int
main(int argc, char**argv)
{
  VECTOR dat;
  byte *dp;
  int left = 0x10000;
  int thislen;
  bool bighead = FALSE;

  {
    int ch;
    while ((ch = getopt(argc, argv, "l")) != -1) {
      switch (ch) {
      case 'l': bighead = TRUE; break;
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
  PrintModeSenseSub(dat, bighead);
  free(dat.dat);
  return 0;
}


