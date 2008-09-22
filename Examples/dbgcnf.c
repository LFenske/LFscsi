#include "scsicmd.h"
#include <stdlib.h>   /* for exit, malloc, strtol */
#include <stdio.h>    /* for fprintf, stderr */

int
main(int argc, char**argv)
{
  char *devfile;
  long  dbgcnf;

  SCSI_HANDLE handle;
  COMMON_PARAMS common;
  VECTOR dat;

  if (argc != 3) {
    fprintf(stderr, "usage: %s <device file> <new dbgcnf value>\n", argv[0]);
    fprintf(stderr, "  change the dbgcnf value in mode page 0x00, subpage 0xF0\n");
    exit(-1);
  }
  devfile = argv[1];
  dbgcnf  = strtol(argv[2], (char**)NULL, 0);

  if (0 != scsi_open(&handle, devfile)) exit(-1);
  common_construct(&common);

  common->dat_size = 12;
  dat.len = 12;
  dat.dat = malloc(dat.len);
  /* header */
  dat.dat[ 0] = 11;           /* remaining length */
  dat.dat[ 1] = 0;
  dat.dat[ 2] = 0;   
  dat.dat[ 3] = 0;            /* block descriptor length */
  /* mode page */
  dat.dat[ 4] = 0x40;         /* subpage format, page_code */
  dat.dat[ 5] = 0xf0;         /* subpage code */
  dat.dat[ 6] = 0x00;         /* page_length[0] */
  dat.dat[ 7] = 0x04;         /* page_length[1] */
  /* mode parameters */
  dat.dat[ 8] = 0x00;         /* page_version */
  dat.dat[ 9] = dbgcnf >> 8;  /* ctl_time_io_secs[0] */
  dat.dat[10] = dbgcnf >> 0;  /* ctl_time_io_secs[1] */
  dat.dat[11] = 0;            /* filler */

  CmdModeSelect(handle, common, 6, dat, TRUE);

  if (common->stt.len > 0) {
    PrintRequestSenseSub(common->stt);
  }
  common_destruct(&common);
  (handle->close)(&handle);
}
