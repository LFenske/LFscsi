/*
 * Copyright (C) 2008  Larry Fenske
 * 
 * This file is part of LFscsi.
 * 
 * LFscsi is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * LFscsi is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.
 */
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

typedef struct {
  CMD cmd;
  const char *name;
  LINE line;
  DIRECTION dir;
  PRINTSUB printer;
  const char *short_help, *long_help;
} DEFINITION;


void usage(void);
void help(COMMON_PARAMS common);


const char *short_help_pre = "[-d device] ";
const char *long_help_common = "\
The following common switches must come before any subcommand-specific switches.\n\
-h        : print help\n\
-d device : device specifier, overrides $SCSI_DEVICE\n\
-z size   : number of bytes of data in or out\n\
-c size   : number of preferred bytes in CDB\n\
-f flavor : device type for command, e.g. SBC, SSC, SMC\n\
-i        : immediate\n\
-t seconds: timeout, in seconds\n\
-r        : raw output, even if to stdout\n\
-v level  : verbosity level\n\
";
VECTOR dat;
char *progname;


#define LINE
#include "allcommands.h"
#undef LINE


DEFINITION def[] = {
#define DEF
#include "allcommands.h"
#undef DEF
  {CMD_LAST, NULL, NULL, DIRECTION_NONE, NULL, NULL}
};


void
usage(void)
{
  DEFINITION *defp;
  fprintf(stderr, "Execute SCSI commands.\nusage: %s <command> <parameters>\ncommands:\n", progname);
  for (defp=def; defp->cmd != CMD_LAST; defp++) {
    fprintf(stderr, "  %s %s\n", defp->name, defp->short_help);
  }
}


void
help(COMMON_PARAMS common)
{
  int cmdnum = -1;
  DEFINITION *defp;

  common->stt.len = 0;

  for (defp=def; defp->cmd != CMD_LAST; defp++) {
    if (common->cmd == defp->cmd) {
      cmdnum = defp-def;
      break;
    }
  }
  if (cmdnum < 0) {
    fprintf(stderr, "internal error - unknown command number: %d\n", common->cmd);
    usage();
    exit(-1);
  }

  fprintf(stderr, "usage: %s %s %s\n", progname, defp->name, defp->short_help);
  fprintf(stderr, "%s", long_help_common);
  fprintf(stderr, "sub-command specific help:\n%s", defp->long_help);
}


int
main(int argc, char**argv)
{
  char *device = getenv("SCSI_DEVICE");
  progname = argv[0];
  SCSI_HANDLE handle;
  COMMON_PARAMS common;
  bool needhelp = FALSE;
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
    usage();
    exit(-1);
  }

  common_construct(&common);
  common->cmd     = def[cmdnum].cmd    ;
  common->dir     = def[cmdnum].dir    ;
  common->printer = def[cmdnum].printer;

  {
    char *flavorstr = NULL;
    bool more = TRUE;
    int ch;
    opterr = 0;
    while (more && (ch = getopt(argc, argv, "hd:z:c:f:it:rv:")) != -1) {
      switch (ch) {
      case 'h': needhelp = TRUE; break;
      case 'd': device = optarg; break;
      case 'z': common->dat_size = strtol(optarg, (char**)NULL, 0); break;
      case 'c': common->cdb_size = strtol(optarg, (char**)NULL, 0); break;
      case 'f': flavorstr = optarg; break;
      case 'i': common->immed = TRUE; break;
      case 't': common->timeout = atof(optarg); break;
      case 'r': raw = TRUE; break;
      case 'v': common->verbose  = strtol(optarg, (char**)NULL, 0); break;
      case '?':
      default:
	more = FALSE;
	optind--;  /* pass this switch on to next stage */
        break;
      }
    }
  }

  if (needhelp) {
    help(common);
    exit(0);
  }

  scsi_open(&handle, device);

  (*(def[cmdnum].line))(handle, common, argc, argv);

  (handle->close)(&handle);

  if (def[cmdnum].dir == DIRECTION_IN) {
    if (!raw && isatty(1)) {
      if (common->printer != NULL) {
        (*(common->printer))(dat);
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
