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


#ifdef ENUM
  CMD_Read,
#endif


#ifdef DEF
  {CMD_Read, "read", LineRead, DIRECTION_IN, PrintDefaultSub, "<SILI-fixed or LBA> <length>", ""},
#endif


#ifdef LINE
#include "CmdRead.h"
#include "PrintDefaultSub.h"

int
LineRead(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  long long param1;
  long length;

  if (argc != optind+2) {
    help(common);
    return -1;
  }

  param1 = strtoull(argv[optind], (char**)NULL, 0); optind++;
  length = strtoul (argv[optind], (char**)NULL, 0); optind++;

  {
    dat = CmdRead(handle, common,
		  param1, length);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdRead_c
#include "CmdRead.h"
#undef  __CmdRead_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdRead(SCSI_HANDLE handle, COMMON_PARAMS common,
         long long param1, long length)   /* size, timeout */
{
  byte cdb[32];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : length;

  if (common->cdb_size == NOSIZE)
      common->cdb_size = 6;

  cdbvec.dat = cdb;
  cdbvec.len = common->cdb_size;
  retval.dat = malloc(thissize);
  retval.len = thissize;

  switch (common->flavor) {
  case NOFLAVOR:
  case SBC:
    {
      long long lba = param1;
      switch (common->cdb_size) {
      case  6:
	if ((lba >> 21) != 0) {
	  /*stub: error message */
	}
	if ((length >> 8) != 0) {
	  /*stub: error message */
	}
	cdb[0] = 0x08;
	cdb[1] = lba >> 16;
	cdb[2] = lba >>  8;
	cdb[3] = lba >>  0;
	cdb[4] = length;
	cdb[5] = 0;
	break;
      case 10:
	if ((lba >> 32) != 0) {
	  /*stub: error message */
	}
	if ((length >> 16) != 0) {
	  /*stub: error message */
	}
	cdb[0] = 0x28;
	cdb[1] = 0; /*stub: bits */
	cdb[2] = lba >> 24;
	cdb[3] = lba >> 16;
	cdb[4] = lba >>  8;
	cdb[5] = lba >>  0;
	cdb[6] = 0; /*stub: group number */
	cdb[7] = length >> 8;
	cdb[8] = length >> 0;
	cdb[9] = 0;
	break;
      case 12:
	if ((lba >> 32) != 0) {
	  /*stub: error message */
	}
#if 0
	if ((length >> 32) != 0) {
	  /*stub: error message */
	}
#endif
	cdb[ 0] = 0xa8;
	cdb[ 1] = 0; /*stub: bits */
	cdb[ 2] = lba >> 24;
	cdb[ 3] = lba >> 16;
	cdb[ 4] = lba >>  8;
	cdb[ 5] = lba >>  0;
	cdb[ 6] = length >> 24;
	cdb[ 7] = length >> 16;
	cdb[ 8] = length >>  8;
	cdb[ 9] = length >>  0;
	cdb[10] = 0; /*stub: group number */
	cdb[11] = 0;
	break;
      case 16:
#if 0
	if ((lba >> 64) != 0) {
	  /*stub: error message */
	}
#endif
#if 0
	if ((length >> 32) != 0) {
	  /*stub: error message */
	}
#endif
	cdb[ 0] = 0x88;
	cdb[ 1] = 0; /*stub: bits */
	cdb[ 2] = lba >> 56;
	cdb[ 3] = lba >> 48;
	cdb[ 4] = lba >> 40;
	cdb[ 5] = lba >> 32;
	cdb[ 6] = lba >> 24;
	cdb[ 7] = lba >> 16;
	cdb[ 8] = lba >>  8;
	cdb[ 9] = lba >>  0;
	cdb[10] = length >> 24;
	cdb[11] = length >> 16;
	cdb[12] = length >>  8;
	cdb[13] = length >>  0;
	cdb[14] = 0; /*stub: group number */
	cdb[15] = 0;
	break;
      default:
	/*stub: error message */
	break;
      }
    }
    break;
  case SSC:
    {
      int sili_fixed = param1;
      switch (common->cdb_size) {
      case  6:
	cdb[0] = 0x08;
	cdb[1] = sili_fixed;
	cdb[2] = length >> 16;
	cdb[3] = length >>  8;
	cdb[4] = length >>  0;
	cdb[5] = 0;
	break;
      default:
	/*stub: error message */
	break;
      }
    }
    break;
  default:
    /*stub: error message */
    break;
  }
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   30.);
  return retval;
}
#endif


