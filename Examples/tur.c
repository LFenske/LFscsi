#include "scsicmd.h"

int
main(int argc, char**argv)
{
  SCSI_HANDLE handle;
  COMMON_PARAMS common;

  if (0 != scsi_open(&handle, "/dev/sda")) exit(-1);
  common_construct(&common);
  CmdTestUnitReady(handle, common);
}

