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
  CMD_PreventAllow,
#endif


#ifdef DEF
  {CMD_PreventAllow, "prevent", LinePrevent, DIRECTION_NONE, NULL, "", ""},
  {CMD_PreventAllow, "allow",   LineAllow  , DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdPreventAllow.h"

int
LinePrevent(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdPreventAllow(handle, common,
                          TRUE);
  }
  return 0;
}

int
LineAllow(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdPreventAllow(handle, common,
                          FALSE);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdPreventAllow_c
#include "CmdPreventAllow.h"
#undef  __CmdPreventAllow_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdPreventAllow(SCSI_HANDLE handle, COMMON_PARAMS common,
                bool prevent)   /* timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x1e;
  cdb[1] = 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = prevent ? 1 : 0;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   1.);
  return retval;
}
#endif


