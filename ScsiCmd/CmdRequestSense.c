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
  CMD_RequestSense,
#endif


#ifdef DEF
  {CMD_RequestSense, "request_sense", LineRequestSense, DIRECTION_IN, PrintRequestSenseSub, "", ""},
  {CMD_RequestSense, "rs"           , LineRequestSense, DIRECTION_IN, PrintRequestSenseSub, "- alias for request_sense", NULL},
#endif


#ifdef LINE
#include "CmdRequestSense.h"
#include "PrintRequestSenseSub.h"

int
LineRequestSense(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdRequestSense(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdRequestSense_c
#include "CmdRequestSense.h"
#undef  __CmdRequestSense_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdRequestSense(SCSI_HANDLE handle, COMMON_PARAMS common)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 18;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x03;
  cdb[1] = 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   1.);
  return retval;
}
#endif


