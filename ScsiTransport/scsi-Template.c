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
/* -*- mode: c -*- */

#include <stdio.h>
#include <stdlib.h>
#include "ScsiTransport.h"


#define debug 0



static int
scsi_close(SCSI_HANDLE *pDevice)
{
  /*stub*/
  free((*pDevice)->context);
  free(*pDevice);
  *pDevice = NULL;
  return 0;
}


static int
scsi_cdb(
         SCSI_HANDLE device,
         DIRECTION direction,
         unsigned char *cdb, int  cdb_len ,
         unsigned char *dat, int *dat_lenp,
         unsigned char *stt, int *stt_lenp,
         float timeout)
{
  int retval = 0;

  if (debug) {
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)cdb, cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)dat,*dat_lenp);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)stt,*stt_lenp);
    fprintf(stderr, "timeout = %f\n", timeout);
  }
  if (debug) {
    int i;
    fprintf(stderr, "SCSI CDB:\n");
    for (i=0; i<cdb_len; i++)
      fprintf(stderr, " %02x", cdb[i]);
    fprintf(stderr, "\n");
  }
  if (direction == DIRECTION_OUT && debug) {
    int i;
    fprintf(stderr, "SCSI data to write:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }

  {
    /*stub*/
  }

  if (direction == DIRECTION_IN && debug && retval != -1) {
    int i;
    fprintf(stderr, "SCSI data read:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }

  if (stt[0] != 0 || retval == -1) {
    if (*stt_lenp > 0) {
      if (debug) {
	int i;
	fprintf(stderr, "SCSI request sense:\n");
	for (i=0; i<*stt_lenp; i++)
	  fprintf(stderr, " %02x", stt[i]);
	fprintf(stderr, "\n");
      }
    }
  }
  if (debug) fprintf(stderr, "retval = %d\n", retval);
  return(retval);
}


static int
scsi_reset(SCSI_HANDLE device, RESET_LEVEL level)
{
  switch (level) {
  case RESET_DEVICE: /*stub*/ break;
  case RESET_BUS   : /*stub*/ break;
  }
  return 0;
}


static int
scsi_scanbus(SCSI_HANDLE device)
{
  /*stub*/
  return 0;
}


int
scsi_open(SCSI_HANDLE *pDevice, void *whatever)
{
  if (debug) fprintf(stderr, "scsi_open '%s'\n", (char*)whatever);
  /*stub*/
  *pDevice = malloc(sizeof(**pDevice));
  (*pDevice)->close   = scsi_close  ;
  (*pDevice)->cdb     = scsi_cdb    ;
  (*pDevice)->reset   = scsi_reset  ;
  (*pDevice)->scanbus = scsi_scanbus;
  (*pDevice)->context = NULL;
  return 0;
}


