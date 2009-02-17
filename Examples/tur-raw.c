#include "ScsiTransport.h"
#include <stdlib.h>  /* for exit */
#include <stdio.h>   /* for printf */

int
main(int argc, char**argv)
{
  SCSI_HANDLE handle;
  unsigned char cdb[ 6] = {0,0,0,0,0,0};  /* test unit ready CDB */
  unsigned char dat[ 1];                  /* no data in or out */
  unsigned char stt[18];                  /* status */
  int cdb_len = sizeof(cdb);
  int dat_len = 0;
  int stt_len = sizeof(stt);
  int retval;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <device file>\n", argv[0]);
    exit(-1);
  }

  if (0 != scsi_open(&handle, argv[1])) exit(-1);

  retval = handle->cdb(handle,
		       DIRECTION_NONE,
		       cdb,  cdb_len,
		       dat, &dat_len,
		       stt, &stt_len,
		       0.0);

  printf("returned %d\n", retval);
  if (stt_len > 0) {
    int i;
    printf("request sense data:");
    for (i=0; i<stt_len; i++)
      printf(" %.2x", stt[i]);
    printf("\n");
  }

  (handle->close)(&handle);
}
