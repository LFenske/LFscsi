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
  CMD_Reset,
#endif


#ifdef DEF
  {CMD_Reset, "reset"      , LineReset      , DIRECTION_NONE, NULL, "", ""},
  {CMD_Reset, "resetdevice", LineResetDevice, DIRECTION_NONE, NULL, "", ""},
#endif


#ifdef LINE
#include "CmdReset.h"

int
LineReset(SCSI_HANDLE handle, COMMON_PARAMS common,
          int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdReset(handle, common, RESET_BUS);
  }
  return 0;
}

int
LineResetDevice(SCSI_HANDLE handle, COMMON_PARAMS common,
                int argc, char**argv)
{
  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdReset(handle, common, RESET_DEVICE);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdReset_c
#include "CmdReset.h"
#undef  __CmdReset_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdReset(SCSI_HANDLE handle, COMMON_PARAMS common,
         RESET_LEVEL level)   /* */
{
  VECTOR retval;
  int status;
  retval.dat = NULL;
  retval.len = 0;
  common->stt.len = 0;
  status = (handle->reset)(handle, level);
  return retval;
}
#endif


