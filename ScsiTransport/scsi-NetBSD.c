/* -*- mode: c -*- */

#include <sys/scsiio.h>
#include <stdio.h>	/* for fprintf */
#include <fcntl.h>	/* for O_RDWR */
#include <string.h>	/* for memset */
#include <unistd.h>	/* for close */
#include "scsiserver.h"


#define debug 0

#define TIMEOUT_DEFAULT 30

#if 0
typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)
#endif


#define B_WRITE 'w'
#define B_READ  'r'

#define MIN(a,b)  (((a) < (b)) ? (a) : (b))


int
send_cdb(devfile, direction, cdb, cdb_len, dat, dat_lenp, stt, stt_lenp, timeout)
     char *devfile;
     int direction;
     unsigned char *cdb,    *dat,     *stt;
     int            cdb_len,*dat_lenp,*stt_lenp;
     int timeout;
{
  int retval = 0;
  scsireq_t req;
  int fd;

  if (debug) {
    int i;
    fprintf(stderr, "devfile = '%s'\n", devfile);
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)cdb, cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)dat,*dat_lenp);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)stt,*stt_lenp);
    fprintf(stderr, "timeout = %d\n", timeout);
    fprintf(stderr, "cdb:\n");
    for (i=0; i<cdb_len; i++)
       fprintf(stderr, " %02x", cdb[i]);
    fprintf(stderr, "\n");
  }

  if (debug && direction == B_WRITE) {
    int i;
    fprintf(stderr, "SCSI data to write:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }


  fd = open(devfile, O_RDWR);
  if (fd < 0) {
     perror("open device");
     retval = -1;
     return retval;
  }

  memset(&req, 0, sizeof(req));
  req.flags    = (direction == B_READ) ? SCCMD_READ : SCCMD_WRITE;
  req.timeout  = timeout * 1000;	/* given seconds, need ms */
  memcpy(req.cmd, cdb, cdb_len);
  req.cmdlen   = cdb_len;
  req.databuf  = dat;
  req.datalen  = *dat_lenp;
  req.senselen = *stt_lenp;

  if (ioctl(fd, SCIOCCOMMAND, &req) == -1)
     perror("SCIOCCOMMAND");

  *dat_lenp = req.datalen_used;
  *stt_lenp = MIN(*stt_lenp, req.senselen_used);
  /*debug* /fprintf(stderr, "NetBSD: datlen = %d\n", *dat_lenp); /**/
  /*debug* /fprintf(stderr, "NetBSD: sttlen = %d\n", *stt_lenp); /**/
  memcpy(stt, req.sense, *stt_lenp);
  /* req.status;   unused */
  /* req.error;    unused */

  if (debug) fprintf(stderr, "retsts = %d\n", req.retsts);
  switch (req.retsts) {
  case SCCMD_OK:
     retval = 0;
     break;
  case SCCMD_TIMEOUT:
     fprintf(stderr, "%s: SCSI command timed out\n", devfile);
     retval = -1;
     break;
  case SCCMD_BUSY:
     fprintf(stderr, "%s: device is busy\n", devfile);
     retval = -1;
     break;
  case SCCMD_SENSE:
     fprintf(stderr, "%s: returned sense\n", devfile);
     retval = -1;
     break;
  default:
     fprintf(stderr, "%s: device had unknown status %x\n", devfile, req.retsts);
     retval = -1;
     break;
  }

  if (debug && direction == B_READ && req.retsts == SCCMD_OK) {
     int i;
     fprintf(stderr, "SCSI data read:\n");
     for (i=0; i<*dat_lenp; i++)
        fprintf(stderr, " %02x", dat[i]);
     fprintf(stderr, "\n");
  }

  if (debug && req.retsts == SCCMD_SENSE) {
     int i;
     fprintf(stderr, "SCSI request sense:\n");
     for (i=0; i<*stt_lenp; i++)
        fprintf(stderr, " %02x", stt[i]);
     fprintf(stderr, "\n");
  }

  close(fd);
  return(retval);
}

