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
  CMD_Template,
#endif


#ifdef DEF
  {CMD_Template, "template", LineTemplate, DIRECTION_TEMPLATE, PrintTemplateSub, "", ""},
#endif


#ifdef LINE
#include "CmdTemplate.h"

int
LineTemplate(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int param = -1;

  if (argc > optind) {
    param = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdTemplate(handle, common,
                      param);
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdTemplate_c
#include "CmdTemplate.h"
#undef  __CmdTemplate_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdTemplate(SCSI_HANDLE handle, COMMON_PARAMS common,
           int param)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 0xff;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = (byte*)malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x??;
  cdb[1] = ;
  cdb[2] = ;
  cdb[3] = ;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
           common->dir,
           cdbvec,
           retval,
           5.);
  return retval;
}
#endif


