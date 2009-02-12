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
  CMD_Locate,
#endif


#ifdef DEF
  {CMD_Locate, "locate", LineLocate, DIRECTION_NONE, NULL, "<objid>", ""},
#endif


#ifdef LINE
#include "CmdLocate.h"

int
LineLocate(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int objid = -1;

  if (argc != optind+1) {
    help(common);
    return -1;
  }

  if (argc > optind) {
    objid = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  {
    dat = CmdLocate(handle, common,
                    objid);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdLocate_c
#include "CmdLocate.h"
#undef  __CmdLocate_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdLocate(SCSI_HANDLE handle, COMMON_PARAMS common,
          int objid)   /* immed, timeout */
{
  byte cdb[10];
  VECTOR cdbvec;
  VECTOR retval;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = NULL;
  retval.len = 0;

  cdb[0] = 0x2b;
  cdb[1] = common->immed ? 1 : 0;  /* BT and CP are always 0 */
  cdb[2] = 0;
  cdb[3] = objid >> 24;
  cdb[4] = objid >> 16;
  cdb[5] = objid >>  8;
  cdb[6] = objid >>  0;
  cdb[7] = 0;
  cdb[8] = 0;
  cdb[9] = 0;
  send_cdb(handle, common,
	   DIRECTION_NONE,
	   cdbvec,
	   retval,
	   120.);
  return retval;
}
#endif


