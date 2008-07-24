/*
 * scsi client stub
 */
/*
 * -d <device>
 * -c <cdblen> [cdb bytes]
 * -w <datlen> [data bytes]
 * -r
 * -t <timeout>
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include "scsi.h"

enum state {
  FREE,
  IN_CDB,
  IN_DAT,
};

typedef char bool;

char *progname;

void dump(char *val, u_int len, char *label);


void usage()
{
  fprintf(stderr, "\
scsi client stub\n\
usage: %s -h<host> -d<device> -c<cdblen> [cdb bytes] { -w<datlen> [data bytes] | -r<datlen> } [ -s<stt_maxlen> ] [ -t<timeout_seconds>]\n\
", progname);
}


int main(int argc, char *argv[])
{
  send_cdb_params params;
  enum state state = FREE;
  int byte_num;
  send_cdb_retval *retval;

  char *host = NULL;

  params.device      = "";
  params.rw          = ' ';
  params.cdb.cdb_len = 0;
  params.dat.dat_len = 0;
  params.dat_maxlen  = 0;
  params.stt_maxlen  = 0;
  params.timeout     = 30;

  progname = argv[0];
  argc--; argv++;

  {
    bool done = FALSE;
    while (!done) {
      char sw;	/* switch character */
      int thisread;	/* number of bytes read this time */
      switch (state) {
      case FREE:
	if (argc == 0) {
	  done = TRUE;
	  break;
	}
	if (**argv != '-') {
	  usage();
	  exit(-1);
	}
	(*argv)++;	/* skip '-' */
	sw = *((*argv)++);	/* remember and skip switch character */
	if (**argv == '\0') {
	  argc--; argv++;
	}
	switch (sw) {
	case 'h':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  host = *argv;
	  argc--; argv++;
	  break;
	case 'd':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  params.device = *argv;
	  argc--; argv++;
	  break;
	case 'c':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  params.cdb.cdb_len = strtol(*argv, (char **)NULL, 0);
	  argc--; argv++;
	  params.cdb.cdb_val = malloc(params.cdb.cdb_len);
	  byte_num = 0;
	  state = IN_CDB;
	  break;
	case 'w':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  params.dat.dat_len = strtol(*argv, (char **)NULL, 0);
	  argc--; argv++;
	  params.dat.dat_val = malloc(params.dat.dat_len);
	  byte_num = 0;
	  state = IN_DAT;
	  if (params.rw != ' ') {
	    usage();
	    exit(-1);
	  }
	  params.rw = 'w';
	  break;
	case 'r':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  params.dat_maxlen = strtol(*argv, (char **)NULL, 0);
	  argc--; argv++;
	  if (params.rw != ' ') {
	    usage();
	    exit(-1);
	  }
	  params.rw = 'r';
	  break;
	case 's':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  params.stt_maxlen = strtol(*argv, (char **)NULL, 0);
	  argc--; argv++;
	  break;
	case 't':
	  if (argc == 0) {
	    usage();
	    exit(-1);
	  }
	  params.timeout = strtol(*argv, (char **)NULL, 0);
	  argc--; argv++;
	  break;
	default:
	  usage();
	  exit(-1);
	  break;
	}
	if (argc == 0)
	  done = TRUE;
	break;
      case IN_CDB:
	if (argc == 0 || **argv == '-') {
	  while (byte_num < params.cdb.cdb_len) {
	    thisread = read(0, params.cdb.cdb_val+byte_num, params.cdb.cdb_len - byte_num);
	    byte_num += thisread;
	  }
	} else {
	  params.cdb.cdb_val[byte_num] = strtol(*argv, (char **)NULL, 0);
	  byte_num++;
	  argc--; argv++;
	}
	if (byte_num == params.cdb.cdb_len) {
	  state = FREE;
	}
	break;
      case IN_DAT:
	if (argc == 0 || **argv == '-') {
	  while (byte_num < params.dat.dat_len) {
	    thisread = read(0, params.dat.dat_val+byte_num, params.dat.dat_len - byte_num);
	    byte_num += thisread;
	  }
	} else {
	  params.dat.dat_val[byte_num] = strtol(*argv, (char **)NULL, 0);
	  byte_num++;
	  argc--; argv++;
	}
	if (byte_num == params.dat.dat_len) {
	  state = FREE;
	}
	break;
      }
    }
  }
  if (host == NULL) {
    fprintf(stderr, "-h must be specified\n");
    usage();
    exit(-1);
  }
  if (params.rw == ' ') {
    fprintf(stderr, "-r or -w must be specified\n");
    usage();
    exit(-1);
  }
  if (params.cdb.cdb_len == 0) {
    fprintf(stderr, "-c must be specified\n");
    usage();
    exit(-1);
  }
  {
    CLIENT *clnt;
    fprintf(stderr, "clnt_create\n");
    clnt = clnt_create(host, SCSI_PROG, SCSI_VERS, "tcp");
    if (clnt == NULL) {
      clnt_pcreateerror(host);
      exit(1);
    }
    fprintf(stderr, "rpc'ing\n");
    retval = send_cdb_1(&params, clnt);
    clnt_destroy( clnt );
  }
  printf("SEND_CDB results\n");
  printf("status: %d\n", retval->status);
  dump(retval->dat.dat_val, retval->dat.dat_len, "dat");
  dump(retval->stt.stt_val, retval->stt.stt_len, "stt");
  return 0;
}


void dump(char *val, u_int len, char *label)
{
  int i;
  printf("%s:\n", label);
  for (i=0; i<len; i++) {
    if (i%16 == 0)
      printf(" ");
    printf(" %.2x", (unsigned char)val[i]);
    if ((i+1)%16 == 0)
      printf("\n");
  }
  if (i%16 != 0)
    printf("\n");
}


