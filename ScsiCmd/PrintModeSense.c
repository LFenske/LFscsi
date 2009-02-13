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

  dp = dat.dat = (byte*)malloc(left);
  while ((thislen = read(0, dp, left))) {
    dp   += thislen;
    left -= thislen;
  }
  dat.len = dp - dat.dat;
  PrintModeSenseSub(dat, bighead);
  free(dat.dat);
  return 0;
}


