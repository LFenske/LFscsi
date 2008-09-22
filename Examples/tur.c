#include "scsicmd.h"
#include <stdlib.h>   /* for exit */

int
main(int argc, char**argv)
{
  SCSI_HANDLE handle;
  COMMON_PARAMS common;

  if (0 != scsi_open(&handle, "/dev/sda")) exit(-1);
  common_construct(&common);

  CmdTestUnitReady(handle, common);

  if (common->stt.len > 0) {
    PrintRequestSenseSub(common->stt);
  }
  common_destruct(&common);
  (handle->close)(&handle);
}
