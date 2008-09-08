#include "scsicmd.h"
#include <stdlib.h>   /* for exit, malloc */

int
main(int argc, char**argv)
{
  SCSI_HANDLE handle;
  COMMON_PARAMS common;
  VECTOR dat;

  if (0 != scsi_open(&handle, "/dev/sda")) exit(-1);
  common_construct(&common);

  common->dat_size = 12;
  dat.len = 12;
  dat.dat = malloc(dat.len);
  dat.dat[ 0] = 0x0b;
  dat.dat[ 1] = 0x00;
  dat.dat[ 2] = 0x00;
  dat.dat[ 3] = 0x00;
  dat.dat[ 4] = 0x40;
  dat.dat[ 5] = 0xf0;
  dat.dat[ 6] = 0x00;
  dat.dat[ 7] = 0x04;
  dat.dat[ 8] = 0x00;
  dat.dat[ 9] = 0x00;
  dat.dat[10] = 0x50;
  dat.dat[11] = 0x00;

  CmdModeSelect(handle, common, 6, dat, TRUE);

  if (common->stt.len > 0) {
    PrintRequestSenseSub(common->stt);
  }
  common_destruct(&common);
  (handle->close)(&handle);
}
