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
  CMD_ReadCapacity,
#endif


#ifdef DEF
  {CMD_ReadCapacity, "readcapacity", LineReadCapacity, DIRECTION_IN, PrintReadCapacitySub16, "", ""},
#endif


#ifdef LINE
#include "CmdReadCapacity.h"
#include "PrintReadCapacitySub.h"

int
LineReadCapacity(SCSI_HANDLE handle, COMMON_PARAMS common,
		 int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdReadCapacity(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdReadCapacity_c
#include "CmdReadCapacity.h"
#undef  __CmdReadCapacity_c

#include "ScsiTransport.h"
#include "common.h"
#include "PrintReadCapacitySub.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdReadCapacity(SCSI_HANDLE handle, COMMON_PARAMS common)   /* size, timeout */
{
  byte cdb[16];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 12;

  if (common->cdb_size == NOSIZE)
    common->cdb_size = 10;

  cdbvec.dat = cdb;
  cdbvec.len = common->cdb_size;

  switch (common->cdb_size) {
  case 10:
    cdb[0] = 0x25;
    cdb[1] = 0;
    cdb[2] = 0; /* logical block address = 0 */
    cdb[3] = 0;
    cdb[4] = 0;
    cdb[5] = 0;
    cdb[6] = 0;
    cdb[7] = 0;
    cdb[8] = 0; /* PMI = 0 */
    cdb[9] = 0;
    common->printer = PrintReadCapacitySub10;
    thissize = 8;
    break;
  case 16:
  case NOSIZE:
    cdb[ 0] = 0x9e;
    cdb[ 1] = 0x10; /* service action */
    cdb[ 2] = 0; /* logical block address = 0 */
    cdb[ 3] = 0;
    cdb[ 4] = 0;
    cdb[ 5] = 0;
    cdb[ 6] = 0;
    cdb[ 7] = 0;
    cdb[ 8] = 0;
    cdb[ 9] = 0;
    cdb[10] = thissize >> 24;
    cdb[11] = thissize >> 16;
    cdb[12] = thissize >>  8;
    cdb[13] = thissize >>  0;
    cdb[14] = 0; /* PMI = 0 */
    cdb[15] = 0;
    common->printer = PrintReadCapacitySub16;
    break;
  default:
    /*stub: error message */
    retval.dat = NULL;
    retval.len = -1;
    common->stt.len = 0;
    return retval;
    break;
  }
  retval.dat = malloc(thissize);
  retval.len = thissize;
  send_cdb(handle, common,
	   common->dir,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif


