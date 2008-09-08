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

  dat.len = 255;
  dat.dat = malloc(dat.len);

  dat = CmdInquiry(handle, common, TRUE, 0x80);

  if (common->stt.len > 0) {
    PrintRequestSenseSub(common->stt);
  } else {
    PrintInquirySub(dat);
  }
  common_destruct(&common);
  (handle->close)(&handle);
}
