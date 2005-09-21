#include "ScsiTransport.h"
#include "common.h"
#include "PrintDefaultSub.h"
#include "PrintRequestSenseSub.h"

#include <unistd.h>   /* for getopt, isatty */
#include <stdlib.h>   /* for malloc, strtol, exit */
#include <limits.h>   /* for strtol */
#include <stdio.h>    /* for printf, fprintf */
#include <string.h>   /* for strcmp */

typedef enum {
#define ENUM
#include "allcommands.h"
#undef ENUM
  CMD_LAST
} CMD;


typedef int (*LINE)(SCSI_HANDLE handle, COMMON_PARAMS common,
                    int argc, char**argv);

typedef void (*PRINTSUB)(VECTOR dat);


typedef struct {
  CMD cmd;
  char *name;
  LINE line;
  DIRECTION dir;
  PRINTSUB printer;
  char *short_help, *long_help;
} DEFINITION;


char *short_help_pre = "[-d device] ";
char *long_help_common = "\
-h       : print help\n\
-d device: device specifier, overrides $SCSI_DEVICE\n\
-z size  : number of bytes of data in or out\n\
-r       : raw output, even if to stdout\n\
";
VECTOR dat;


#define LINE
#include "allcommands.h"
#undef LINE


DEFINITION def[] = {
#define DEF
#include "allcommands.h"
#undef DEF
  {CMD_LAST, NULL, NULL, DIRECTION_NONE, NULL, NULL}
};


int
main(int argc, char**argv)
{
  /*char *progname = argv[0];*/
  char *device = getenv("SCSI_DEVICE");
  SCSI_HANDLE handle;
  COMMON_PARAMS common;
  bool help = FALSE;
  bool raw = FALSE;

  int cmdnum = -1;

  { // figure out which command this is and set cmdnum
    char *cmdname;
    DEFINITION *p;
    cmdname = *(++argv); --argc;
    for (p=def; p->cmd != CMD_LAST; p++) {
      if (strcmp(cmdname, p->name) == 0) {
        cmdnum = p-def;
        break;
      }
    }
  }
  if (cmdnum < 0) {
    fprintf(stderr, "unknown command: %s\n", *argv);
    /*stub: usage(progname)*/
    exit(-1);
  }

  common_construct(&common);

  {
    int ch;
    while ((ch = getopt(argc, argv, "hd:z:ir")) != -1) {
      switch (ch) {
      case 'h': help = TRUE; break;
      case 'd': device = optarg; break;
      case 'z': common->size = strtol(optarg, (char**)NULL, 0); break;
      case 'i': common->immed = TRUE; break;
      case 'r': raw = TRUE; break;
      case '?':
      default:
        {
          /*stub: usage(progname);*/
          exit(-1);
        }
        break;
      }
    }
  }
  argc -= optind;
  argv += optind;

  scsi_open(&handle, device);

  (*(def[cmdnum].line))(handle, common, argc, argv);

  (handle->close)(&handle);

  if (def[cmdnum].dir == DIRECTION_IN) {
    if (!raw && isatty(1)) {
      if (def[cmdnum].printer != NULL) {
        (*(def[cmdnum].printer))(dat);
      } else {
        PrintDefaultSub(dat);
      }
    } else {
      write(1, dat.dat, dat.len);
    }
    free(dat.dat);
  }

  if (common->stt.len > 0) {
#if 0
    int i;
    printf("sense:");
    for (i=0; i<common->stt.len; i++)
      printf(" %.2x", (unsigned char)(common->stt.dat[i]));
    printf("\n");
#else
    PrintRequestSenseSub(common->stt);
#endif
  }

  common_destruct(&common);
  return 0;
}
