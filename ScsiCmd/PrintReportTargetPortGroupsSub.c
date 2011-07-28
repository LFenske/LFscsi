/*
 * Copyright (C) 2011  Larry Fenske
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
#define __PrintReportTargetPortGroupsSub_c

#include "common.h"
#include "PrintReportTargetPortGroupsSub.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy, strcat */
#include <ctype.h>    /* for isprint */


void
PrintReportTargetPortGroupsSub(VECTOR dat)
{
  unsigned long datalen = 0;
  int idx = 0;
  printf("Report Target Port Groups\n");
  do {
    if (dat.len < 4) break;
    datalen =
      ((unsigned long)dat.dat[0] << 24) |
      ((unsigned long)dat.dat[1] << 16) |
      ((unsigned long)dat.dat[2] <<  8) |
      ((unsigned long)dat.dat[3] <<  0) |
      0;
    datalen += 4;
    idx = 4;
    while (idx < datalen) {
      /* target port group descriptors */
      char *aas;
      char *stat;
      int left;

      printf("  Target Port Group: %d\n", ((dat.dat[idx+2]<<8) | (dat.dat[idx+3]<<0)));
      printf("    %s", (dat.dat[idx+0] & 0x80) ? "preferred" : "not preferred");
      switch (dat.dat[idx+0] & 0x0f) {
      case 0x0: aas="Active/optimized"            ; break;
      case 0x1: aas="Active/non-optimized"        ; break;
      case 0x2: aas="Standby"                     ; break;
      case 0x3: aas="Unavailable"                 ; break;
      case 0xf: aas="Transitioning between states"; break;
      default : aas="Reserved"                    ; break;
      }
      printf("    %s\n", aas);
      printf("    SUP");
      printf(" T:%d" , (dat.dat[idx+1] >> 7) & 1);
      printf(" U:%d" , (dat.dat[idx+1] >> 3) & 1);
      printf(" S:%d" , (dat.dat[idx+1] >> 2) & 1);
      printf(" AN:%d", (dat.dat[idx+1] >> 1) & 1);
      printf(" AO:%d", (dat.dat[idx+1] >> 0) & 1);
      printf("\n");
      switch (dat.dat[idx+5]) {
      case 0 : stat="no status"          ; break;
      case 1 : stat="altered by STPG"    ; break;
      case 2 : stat="altered by behavior"; break;
      default: stat="reserved"           ; break;
      }
      printf("    status code: 0x%.2x %s\n", dat.dat[idx+5], stat);
      left = dat.dat[idx+7];
      idx += 8;
      printf("    ports:");
      while (left > 0) {
        printf(" 0x%x", (dat.dat[idx+2]<<8) | (dat.dat[idx+3]<<0));
	idx += 4;
	left--;
      }
      printf("\n");
    }
  } while(0);
}


