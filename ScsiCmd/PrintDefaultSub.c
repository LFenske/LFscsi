/*
 * Copyright (C) 2008  Larry Fenske
 * 
 * This file is part of LFscsi.
 * 
 * LFscsi is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * LFscsi is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.
 */
#define __PrintDefaultSub_c

#include "common.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy */
#include <ctype.h>    /* for isprint */

#ifdef __cplusplus
using namespace LFscsi;
#endif


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


