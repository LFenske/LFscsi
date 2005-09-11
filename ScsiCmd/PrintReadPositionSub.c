#define __PrintReadPositionSub_c

#include "common.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy */


char *
TF(int bit)
{
  return bit ? "true" : "false";
}


char *
val(int bit)
{
  return bit ? " (invalid)" : "";
}


void
PrintReadPositionSub(VECTOR dat)
{
  unsigned char *d = dat.dat;
  int flags      = d[ 0];
  int partnum    = d[ 1];
  int blockfirst = (d[ 4] << 24) | (d[ 5] << 16) | (d[ 6] <<  8) | (d[ 7] << 0);
  int blocklast  = (d[ 8] << 24) | (d[ 9] << 16) | (d[10] <<  8) | (d[11] << 0);
  int numblocks  = (d[12] << 24) | (d[13] << 16) | (d[14] <<  8) | (d[15] << 0);
  int numbytes   = (d[16] << 24) | (d[17] << 16) | (d[18] <<  8) | (d[19] << 0);
  int BOP  = (flags >> 7) & 1;
  int EOP  = (flags >> 6) & 1;
  int BCU  = (flags >> 5) & 1;
  int BYCU = (flags >> 4) & 1;
  int BPU  = (flags >> 2) & 1;
  int PERR = (flags >> 1) & 1;
  printf("Read Position:\n");
  printf("  BOP : %s\n", TF(BOP ));
  printf("  EOP : %s\n", TF(EOP ));
  printf("  BCU : %s\n", TF(BCU ));
  printf("  BYCU: %s\n", TF(BYCU));
  printf("  BPU : %s\n", TF(BPU ));
  printf("  PERR: %s\n", TF(PERR));
  printf("  First Block Location      : %d%s\n", blockfirst, val(BPU ));
  printf("  Last  Block Location      : %d%s\n", blocklast , val(BPU ));
  printf("  Number of blocks in buffer: %d%s\n", numblocks , val(BCU ));
  printf("  Number of bytes  in buffer: %d%s\n", numbytes  , val(BYCU));
}


