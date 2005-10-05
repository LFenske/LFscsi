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
     printf("cdb:");
     for (i=0; i<cdb.len; i++)
        printf(" %.2x", cdb.dat[i]);
     printf("\n");
  }
  status = (device->cdb)(device,
                         dir,
                         cdb.dat, cdb.len,
                         dat.dat, &dat.len,
                         common->stt.dat, &common->stt.len,
                         timeout);
}


