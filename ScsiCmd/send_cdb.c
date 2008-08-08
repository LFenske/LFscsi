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


