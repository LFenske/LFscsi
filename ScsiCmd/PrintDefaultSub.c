#define __PrintDefaultSub_c

#include "common.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy */
#include <ctype.h>    /* for isprint */


void
PrintDefaultSub(VECTOR dat)
{
   int lineaddr;
   for (lineaddr=0; lineaddr<dat.len; lineaddr+=16) {
      int addr;
      int eoladdr;
      printf("%.8x", lineaddr);
      for (addr=lineaddr; addr<lineaddr+16; addr++) {
         if (addr%8 == 0) printf(" ");
         if (addr < dat.len) printf(" %.2x", dat.dat[addr]);
         else                printf("   ");
      }
      eoladdr = lineaddr + 16;
      if (eoladdr > dat.len)
         eoladdr = dat.len;
      printf("  |");
      for (addr=lineaddr; addr<eoladdr; addr++) {
         char c = dat.dat[addr];
         if (isprint(c)) printf("%c", c);
         else            printf(".");
      }
      printf("|\n");
   }
   printf("%.8x\n", dat.len);
}


