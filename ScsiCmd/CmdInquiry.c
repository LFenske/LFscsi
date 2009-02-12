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
  CMD_Inquiry,
#endif


#ifdef DEF
  {CMD_Inquiry, "inquiry", LineInquiry, DIRECTION_IN , PrintInquirySub, "[-z size] [-r] [page code]", ""},
  {CMD_Inquiry, "inq"    , LineInquiry, DIRECTION_IN , PrintInquirySub, "- alias for inquiry", NULL},
#endif


#ifdef LINE
#include "CmdInquiry.h"
#include "PrintInquirySub.h"

int
LineInquiry(SCSI_HANDLE handle, COMMON_PARAMS common,
            int argc, char**argv)
{
  int page = -1;

  if (argc > optind) {
    page = strtol(argv[optind], (char**)NULL, 0);
    optind++;
  }

  if (argc > optind) {
    help(common);
    return -1;
  }

  {
    dat = CmdInquiry(handle, common,
                     (page == -1) ? 0 : 1,
                     (page == -1) ? 0 : page
                     );
  }
  return 0;
}
#endif


#ifdef COMMAND
#define __CmdInquiry_c
#include "CmdInquiry.h"
#undef  __CmdInquiry_c

#include "ScsiTransport.h"
#include "common.h"
#include <stdlib.h>   /* for malloc */

VECTOR
CmdInquiry(SCSI_HANDLE handle, COMMON_PARAMS common,
           bool evpd, int page_code)   /* size, timeout */
{
  byte cdb[6];
  VECTOR cdbvec;
  VECTOR retval;
  int thissize = (common->dat_size != NOSIZE) ? common->dat_size : 0xff;

  cdbvec.dat = cdb;
  cdbvec.len = sizeof(cdb);
  retval.dat = malloc(thissize);
  retval.len = thissize;

  cdb[0] = 0x12;
  cdb[1] = evpd;
  cdb[2] = page_code;
  cdb[3] = 0;
  cdb[4] = thissize;
  cdb[5] = 0;
  send_cdb(handle, common,
	   DIRECTION_IN,
	   cdbvec,
	   retval,
	   5.);
  return retval;
}
#endif


