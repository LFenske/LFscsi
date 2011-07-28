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


#ifdef ENUM
  CMD_ReportTargetPortGroups,
#endif


#ifdef DEF
  {CMD_ReportTargetPortGroups, "reporttargetportgroups", LineReportTargetPortGroups, DIRECTION_IN, PrintReportTargetPortGroupsSub, "", ""},
  {CMD_ReportTargetPortGroups, "rtpg"                  , LineReportTargetPortGroups, DIRECTION_IN, PrintReportTargetPortGroupsSub, "", ""},
#endif


#ifdef LINE
#include "CmdReportTargetPortGroups.h"
#include "PrintReportTargetPortGroupsSub.h"

int
LineReportTargetPortGroups(SCSI_HANDLE handle, COMMON_PARAMS common,
		 int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdReportTargetPortGroups(handle, common);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdReportTargetPortGroups_c
#include "CmdReportTargetPortGroups.h"
#undef  __CmdReportTargetPortGroups_c

#include "ScsiTransport.h"
#include "common.h"
#include "PrintReportTargetPortGroupsSub.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdReportTargetPortGroups(SCSI_HANDLE handle, COMMON_PARAMS common)   /* size, timeout */
{
  byte cdb[16];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 4;

  common->cdb_size = 12;

  cdbvec.dat = cdb;
  cdbvec.len = common->cdb_size;

  switch (common->cdb_size) {
  case 12:
  case NOSIZE:
    cdb[ 0] = 0xa3; /* Maintenance In */
    cdb[ 1] = 0x0a; /* Service Action */
    cdb[ 2] = 0;
    cdb[ 3] = 0;
    cdb[ 4] = 0;
    cdb[ 5] = 0;
    cdb[ 6] = thissize >> 24;
    cdb[ 7] = thissize >> 16;
    cdb[ 8] = thissize >>  8;
    cdb[ 9] = thissize >>  0;
    cdb[10] = 0;
    cdb[11] = 0;
    common->printer = PrintReportTargetPortGroupsSub;
    break;
  default:
    /*stub: error message */
    retval.dat = NULL;
    retval.len = -1;
    common->stt.len = 0;
    return retval;
    break;
  }
  retval.dat = (byte*)malloc(thissize);
  retval.len = thissize;
  send_cdb(handle, common,
	   common->dir,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif


