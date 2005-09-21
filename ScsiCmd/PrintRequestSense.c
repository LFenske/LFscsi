#include "common.h"
#include "PrintRequestSenseSub.h"
#include <stdlib.h>   /* for malloc */
#include <unistd.h>   /* for read */


int
main()
{
  VECTOR dat;
  char *dp;
  int left = 0x10000;
  int thislen;
  dp = dat.dat = malloc(left);
  while ((thislen = read(0, dp, left))) {
    dp   += thislen;
    left -= thislen;
  }
  dat.len = dp - (char*)dat.dat;
  PrintRequestSenseSub(dat);
  free(dat.dat);
  return 0;
}


