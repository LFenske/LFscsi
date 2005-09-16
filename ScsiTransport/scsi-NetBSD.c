/* -*- mode: c -*- */

#include <sys/scsiio.h>
#include <stdio.h>	/* for fprintf */
#include <fcntl.h>	/* for O_RDWR */
#include <string.h>	/* for memset */
#include <unistd.h>	/* for close */
#include <stdlib.h>     /* for free, malloc */
#include "ScsiTransport.h"


#define debug 0

#define TIMEOUT_DEFAULT 30

#define B_WRITE 'w'
#define B_READ  'r'

#define MIN(a,b)  (((a) < (b)) ? (a) : (b))


#define GET_FD(__handle)  (*(int*)((__handle)->context))


static int
scsi_close(SCSI_HANDLE *pDevice)
{
  int fd = GET_FD(*pDevice);
  close(fd);
  free((*pDevice)->context);
  free(*pDevice);
  *pDevice = NULL;
  return 0;
}


static int
scsi_cdb(
         SCSI_HANDLE device,
         DIRECTION direction,
         unsigned char *cdb, int  cdb_len ,
         unsigned char *dat, int *dat_lenp,
         unsigned char *stt, int *stt_lenp,
         float timeout)
{
  int retval = 0;
  scsireq_t req;
  int fd = GET_FD(device);

  if (fd < 0) {
     if (debug) {
        fprintf(stderr, "aborting because fd = %d\n", fd);
     }
     return -1;
  }
  if (debug) {
    fprintf(stderr, "fd = %d\n", fd);
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)cdb, cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)dat,*dat_lenp);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)stt,*stt_lenp);
    fprintf(stderr, "timeout = %f\n", timeout);
  }
  if (debug) {
    int i;
    fprintf(stderr, "SCSI CDB:\n");
    for (i=0; i<cdb_len; i++)
      fprintf(stderr, " %02x", cdb[i]);
    fprintf(stderr, "\n");
  }
  if (direction == DIRECTION_OUT && debug) {
    int i;
    fprintf(stderr, "SCSI data to write:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }


  memset(&req, 0, sizeof(req));
  req.flags    =
       (direction==DIRECTION_OUT) ? SCCMD_WRITE :
       ((direction==DIRECTION_IN) ? SCCMD_READ  :
        SCCMD_READ);
  req.timeout  = (int)(timeout * 1000);	/* given seconds, need ms */
  memcpy(req.cmd, cdb, cdb_len);
  req.cmdlen   = cdb_len;
  req.databuf  = dat;
  req.datalen  = *dat_lenp;
  req.senselen = *stt_lenp;

  if (ioctl(fd, SCIOCCOMMAND, &req) == -1)
     perror("SCIOCCOMMAND");

  *dat_lenp = req.datalen_used;
  *stt_lenp = MIN(*stt_lenp, req.senselen_used);
  /*debug* /fprintf(stderr, "NetBSD: datlen = %d\n", *dat_lenp); / **/
  /*debug* /fprintf(stderr, "NetBSD: sttlen = %d\n", *stt_lenp); / **/
  memcpy(stt, req.sense, *stt_lenp);
  /* req.status;   unused */
  /* req.error;    unused */

  if (debug) fprintf(stderr, "retsts = %d\n", req.retsts);
  switch (req.retsts) {
  case SCCMD_OK:
     retval = 0;
     break;
  case SCCMD_TIMEOUT:
     fprintf(stderr, "SCSI command timed out\n");
     retval = -1;
     break;
  case SCCMD_BUSY:
     fprintf(stderr, "device is busy\n");
     retval = -1;
     break;
  case SCCMD_SENSE:
     fprintf(stderr, "returned sense\n");
     retval = -1;
     break;
  default:
     fprintf(stderr, "device had unknown status %x\n", req.retsts);
     retval = -1;
     break;
  }

  if (debug && direction == DIRECTION_IN && req.retsts == SCCMD_OK) {
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
  if (debug) fprintf(stderr, "retval = %d\n", retval);
  return(retval);
}


static int
scsi_reset(SCSI_HANDLE device, RESET_LEVEL level)
{
  int fd = GET_FD(device);
  int retval = 0;
  if (fd < 0) {
     if (debug) {
        fprintf(stderr, "aborting because fd = %d\n", fd);
     }
     return -1;
  }
  switch (level) {
  case RESET_DEVICE: retval = ioctl(fd, SCIOCRESET  , NULL); break;
  case RESET_BUS   : retval = ioctl(fd, SCBUSIORESET, NULL); break;
  default: fprintf(stderr, "scsi_reset: unknown level = %d\n", level); return -1; break;
  }
  if (retval < 0) {
     perror("scsi_reset");
     return -1;
  }
  return 0;
}


static int
scsi_scanbus(SCSI_HANDLE device)
{
  /*stub*/
  return 0;
}


int
scsi_open(SCSI_HANDLE *pDevice, void *whatever)
{
  int *pfd;
  int retval = 0;
  if (debug) fprintf(stderr, "scsi_open '%s'\n", (char*)whatever);
  *pDevice = malloc(sizeof(**pDevice));
  pfd      = malloc(sizeof( *pfd    ));
  *pfd = open((char*)whatever, O_RDWR);
  if (*pfd < 0) {
    perror("open device");
    retval = -1;
  }
  (*pDevice)->close   = scsi_close  ;
  (*pDevice)->cdb     = scsi_cdb    ;
  (*pDevice)->reset   = scsi_reset  ;
  (*pDevice)->scanbus = scsi_scanbus;
  (*pDevice)->context = pfd;
  return retval;
}


