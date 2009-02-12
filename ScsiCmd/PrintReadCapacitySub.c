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
#define __PrintReadCapacitySub_c

#include "common.h"
#include "PrintReadCapacitySub.h"
#include <stdio.h>    /* for printf, NULL */
#include <stdlib.h>   /* for malloc */
#include <string.h>   /* for memcpy, strcat */
#include <ctype.h>    /* for isprint */


void
PrintReadCapacitySub(VECTOR dat, bool longdata)
{
  if (longdata) PrintReadCapacitySub16(dat);
  else          PrintReadCapacitySub10(dat);
}


void
PrintReadCapacitySub10(VECTOR dat)
{
  do {
    if (dat.len < 4) break;
    {
      unsigned long lba =
	((unsigned long)dat.dat[0] << 24) |
	((unsigned long)dat.dat[1] << 16) |
	((unsigned long)dat.dat[2] <<  8) |
	((unsigned long)dat.dat[3] <<  0) |
	0;
      printf("logical block address: 0x%.8lx (%lu)\n", lba, lba);
    }
    if (dat.len < 8) break;
    {
      unsigned long len =
	((unsigned long)dat.dat[4] << 24) |
	((unsigned long)dat.dat[5] << 16) |
	((unsigned long)dat.dat[6] <<  8) |
	((unsigned long)dat.dat[7] <<  0) |
	0;
      printf("logical block length : 0x%.8lx (%lu)\n", len, len);
    }
  } while(0);
}


void
PrintReadCapacitySub16(VECTOR dat)
{
  do {
    if (dat.len < 8) break;
    {
      unsigned long long lba =
	((unsigned long long)dat.dat[ 0] << 56) |
	((unsigned long long)dat.dat[ 1] << 48) |
	((unsigned long long)dat.dat[ 2] << 40) |
	((unsigned long long)dat.dat[ 3] << 32) |
	((unsigned long long)dat.dat[ 4] << 24) |
	((unsigned long long)dat.dat[ 5] << 16) |
	((unsigned long long)dat.dat[ 6] <<  8) |
	((unsigned long long)dat.dat[ 7] <<  0) |
	0;
      printf("logical block address: 0x%.16llx (%llu)\n", lba, lba);
    }
    if (dat.len < 12) break;
    {
      unsigned long len =
	((unsigned long)dat.dat[ 8] << 24) |
	((unsigned long)dat.dat[ 9] << 16) |
	((unsigned long)dat.dat[10] <<  8) |
	((unsigned long)dat.dat[11] <<  0) |
	0;
      printf("logical block length : 0x%.8lx (%lu)\n", len, len);
    }
    if (dat.len < 13) break;
    if (dat.dat[12] & 1) {
      int p_type = (dat.dat[12] >> 1) & 0x7;
      if (p_type < 3) {
	printf("protection type: %d\n", p_type + 1);
      } else {
	printf("protection type: undefined\n");
      }
    } else {
      printf("protection type: %d\n", 0);
    }
    if (dat.len < 14) break;
    printf("logical blocks per physical block exponent: %d\n", dat.dat[13] & 0xf);
    if (dat.len < 16) break;
    {
      int lalba = (
		   (dat.dat[14] << 8) |
		   (dat.dat[15] << 0) |
		   0
		   ) & 0x3fff;
      printf("lowest aligned logical block address: 0x%x (%d)\n", lalba, lalba);
    }
  } while(0);
}


