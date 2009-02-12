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
#define __send_cdb_c

#include "send_cdb.h"
#include <stdio.h>   /* for printf */


void
send_cdb(SCSI_HANDLE device,
         COMMON_PARAMS common,
         DIRECTION dir,
         VECTOR cdb,
         VECTOR dat,
         float default_timeout)
{
  int status;
  float timeout = (common->timeout > 0.) ? common->timeout : default_timeout;
  if (common->verbose) {
     int i;
     fprintf(stderr, "cdb:");
     for (i=0; i<cdb.len; i++)
        fprintf(stderr, " %.2x", cdb.dat[i]);
     fprintf(stderr, "\n");
  }
  status = (device->cdb)(device,
                         dir,
                         cdb.dat, cdb.len,
                         dat.dat, &dat.len,
                         common->stt.dat, &common->stt.len,
                         timeout);
}


