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
  CMD_LoadUnload,
#endif


#ifdef DEF
  {CMD_LoadUnload, "load"  , LineLoad  , DIRECTION_NONE, NULL, "[0|1]", "1: load; 0: unload"},
  {CMD_LoadUnload, "unload", LineUnload, DIRECTION_NONE, NULL, "[0|1]", "1: load; 0: unload"},
#endif


#ifdef LINE
#include "CmdLoadUnload.h"

int
LineLoadUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
               int argc, char**argv,
               int toLoad)
{
  if (argc > optind) {
    toLoad = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdLoadUnload(handle, common,
                        toLoad);
  }
  return 0;
}


int
LineLoad(SCSI_HANDLE handle, COMMON_PARAMS common,
         int argc, char**argv)
{
  return LineLoadUnload(handle, common, argc, argv, 1);
}


int
LineUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
           int argc, char**argv)
{
  return LineLoadUnload(handle, common, argc, argv, 0);
}


#endif


#ifdef COMMAND
#define __CmdLoadUnload_c
#include "CmdLoadUnload.h"
#undef  __CmdLoadUnload_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdLoadUnload(SCSI_HANDLE handle, COMMON_PARAMS common,
              int toLoad)   /* immed, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x1b;
  cdb[1] = common->immed ? 1 : 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = toLoad;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   120.);
  return retval;
}
#endif


