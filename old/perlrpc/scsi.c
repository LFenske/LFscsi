/* -*- mode: c -*- */

#include "EXTERN.h"
#include "perl.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <rpc/rpc.h>

#include "scsirpc/scsi.h"


#define debug 0

#define TIMEOUT_DEFAULT 30

#define B_WRITE 'w'
#define B_READ  'r'


int
send_cdb(devfile, direction, cdb, cdb_len, dat, dat_lenp, stt, stt_lenp, timeout)
     char *devfile;
     int direction;
     unsigned char *cdb,    *dat,     *stt;
     int            cdb_len,*dat_lenp,*stt_lenp;
     int timeout;
{
  int status = 0;

  if (debug) {
    fprintf(stderr, "devfile = '%s'\n", devfile);
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)cdb, cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)dat,*dat_lenp);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)stt,*stt_lenp);
    fprintf(stderr, "timeout = %d\n", timeout);
    if (direction == B_WRITE) {
      int i;
      fprintf(stderr, "SCSI data to write:\n");
      for (i=0; i<*dat_lenp; i++)
	fprintf(stderr, " %02x", dat[i]);
      fprintf(stderr, "\n");
    }
  }


  {
    send_cdb_params  params;
    send_cdb_retval *retval;
    char *hostdev, *host, *dev, *p;

    hostdev = strdup(devfile);
    p = strchr(hostdev, ':');
    if (p == NULL) {
      host = "localhost";
      dev  = hostdev;
    } else {
      host = hostdev;
      *p = '\0';
      dev = p+1;
    }
    params.device = dev;
    params.rw = direction;
    params.cdb.cdb_val = cdb;
    params.cdb.cdb_len = cdb_len;
    params.dat.dat_val = dat;
    params.dat.dat_len = (direction==B_WRITE) ? *dat_lenp : 0;
    params.dat_maxlen  = (direction==B_READ ) ? *dat_lenp : 0;
    params.stt_maxlen = *stt_lenp;
    params.timeout = timeout;

    {
#include <time.h>
      CLIENT *clnt;
      struct timeval timeval;

      clnt = clnt_create(host, SCSI_PROG, SCSI_VERS, "tcp");
      if (clnt == NULL) {
	clnt_pcreateerror(host);
	exit(1);
      }

      /* set RPC timeout to SCSI timeout plus some seconds */
      /* this overrides the timeout parameter in clnt_call in send_cdb_1 */
      timeval.tv_sec = timeout + 5;
      timeval.tv_usec = 0;
      clnt_control(clnt, CLSET_TIMEOUT, (char*)&timeval);

      retval = send_cdb_1(&params, clnt);

      clnt_destroy( clnt );
    }

    status    = retval->status;
    *dat_lenp = retval->dat.dat_len;
    if (direction==B_READ)
      memcpy(dat, retval->dat.dat_val, retval->dat.dat_len);
    *stt_lenp = retval->stt.stt_len;
    memcpy(stt, retval->stt.stt_val, retval->stt.stt_len);
  }


  if (debug) {
    fprintf(stderr, "status = %d\n", status);
    if (direction == B_READ && status != -1) {
      int i;
      fprintf(stderr, "SCSI data read:\n");
      for (i=0; i<*dat_lenp; i++)
	fprintf(stderr, " %02x", dat[i]);
      fprintf(stderr, "\n");
    }
    if (*stt_lenp > 0) {
      int i;
      fprintf(stderr, "SCSI request sense:\n");
      for (i=0; i<*stt_lenp; i++)
	fprintf(stderr, " %02x", stt[i]);
      fprintf(stderr, "\n");
    }
  }

  return(status);
}

