#include "CmdInquiry.h"
#include "ScsiTransport.h"
#include "common.h"

#include <unistd.h>   /* for getopt, isatty */
#include <stdlib.h>   /* for strtol, exit */
#include <limits.h>   /* for strtol */
#include <stdio.h>    /* for printf, fprintf */


void
usage(char *progname)
{
  fprintf(stderr, "usage: %s [-d device] [-z size] [-r] [page code]\n", progname);
}


int
main(int argc, char**argv)
{
  char *progname = argv[0];
  char *device = getenv("SCSI_DEVICE");
  COMMON_PARAMS common;
  VECTOR dat;
  bool raw = FALSE;

  int page = -1;

  common_construct(&common);

  {
    int ch;
    while ((ch = getopt(argc, argv, "d:z:r")) != -1) {
      switch (ch) {
      case 'd':
        {
          device = optarg;
        }
        break;
      case 'z':
        {
          common->size = strtol(optarg, (char**)NULL, 0);
        }
        break;
      case 'r':
        {
	  raw = TRUE;
	}
	break;
      case '?':
      default:
        {
          usage(progname);
          exit(-1);
        }
        break;
      }
    }
  }
  argc -= optind;
  argv += optind;

  if (argc > 0) {
    page = strtol(argv[0], (char**)NULL, 0);
    argc--;
    argv++;
  }

  if (argc > 0) {
    usage(progname);
    exit(-1);
  }

  {
    SCSI_HANDLE handle;
    scsi_open(&handle, device);
    dat = CmdInquiry(handle, common,
                     (page == -1) ? 0 : 1,
                     (page == -1) ? 0 : page
                     );
    (handle->close)(&handle);
  }

  if (common->stt.len > 0) {
    int i;
    printf("sense:");
    for (i=0; i<common->stt.len; i++)
      printf(" %.2x", (unsigned char)(common->stt.dat[i]));
    printf("\n");
  }
  if (raw || !isatty(1)) {
    write(1, dat.dat, dat.len);
  } else {
    int i;
    printf("inquiry data:");
    for (i=0; i<dat.len; i++)
      printf(" %.2x", (unsigned char)(dat.dat[i]));
    printf("\n");
  }

  free(dat.dat);
  common_destruct(&common);
  return 0;
}
