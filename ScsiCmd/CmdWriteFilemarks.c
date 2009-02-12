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
  CMD_WriteFilemarks,
#endif


#ifdef DEF
  {CMD_WriteFilemarks, "write_filemarks", LineWriteFilemarks, DIRECTION_NONE, NULL, "[number of marks]", ""},
  {CMD_WriteFilemarks, "wf"             , LineWriteFilemarks, DIRECTION_NONE, NULL, "- alias for write_filemarks", NULL},
#endif


#ifdef LINE
#include "CmdWriteFilemarks.h"

int
LineWriteFilemarks(SCSI_HANDLE handle, COMMON_PARAMS common,
                   int argc, char**argv)
{
  int count = 1;

  if (argc > optind) {
    count = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdWriteFilemarks(handle, common,
                            count);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdWriteFilemarks_c
#include "CmdWriteFilemarks.h"
#undef  __CmdWriteFilemarks_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdWriteFilemarks(SCSI_HANDLE handle, COMMON_PARAMS common,
                  int count)   /* immed, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x10;
  cdb[1] = common->immed ? 1 : 0;
  cdb[2] = 0;
  cdb[3] = 0;
  cdb[4] = count;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   120.);
  return retval;
}
#endif


