/* -*- mode: c -*- */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <scsi/sg.h>
#include <fcntl.h>
#include <sys/ioctl.h>	/* for ioctl */
#include <string.h>	/* for memset */
#include <sys/param.h>	/* for HZ */
#include "ScsiTransport.h"


#define debug 0

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
  sg_io_hdr_t hdr;
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


  {
    hdr.interface_id = 'S';
    hdr.dxfer_direction =
       (direction==DIRECTION_OUT) ? SG_DXFER_TO_DEV :
       ((direction==DIRECTION_IN) ? SG_DXFER_FROM_DEV :
        SG_DXFER_NONE);
    hdr.cmd_len = cdb_len;
    hdr.mx_sb_len = *stt_lenp;
    hdr.iovec_count = 0;  /* 0: no scatter-gather */
    hdr.dxfer_len = *dat_lenp;
    hdr.dxferp = dat;
    hdr.cmdp = cdb;
    hdr.sbp = stt;
    hdr.timeout = (int)(timeout*1000);  /* value is needed in ms */
    hdr.flags = 0;
    hdr.pack_id = -1;
    hdr.usr_ptr = NULL;

    if (debug) fprintf(stderr, "direction = %d\n", hdr.dxfer_direction);

    if (0 && debug) {
       int i;
       fprintf(stderr, "fd = %d\n", fd);
       fprintf(stderr, "hdr =");
       for (i=0; i<sizeof(hdr); i++) {
          fprintf(stderr, " %.2x", ((unsigned char*)&hdr)[i]);
       }
       fprintf(stderr, "\n");
    }

    if (retval != -1) {
      if (ioctl(fd, SG_IO, &hdr) == -1) {
        perror("sg_xfer(ioctl)");
        retval = -1;
      }
    }
    if (retval != -1) {
      if (debug) fprintf(stderr, "sg_xfer status = 0x%x\n", hdr.status);
      retval = hdr.status;
    }
  }

  *stt_lenp = hdr.sb_len_wr;
  *dat_lenp = hdr.dxfer_len - hdr.resid;

  if (debug && direction == DIRECTION_IN && retval != -1) {
    int i;
    fprintf(stderr, "SCSI data read:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }

  if (stt[0] != 0 || retval == -1) {
    if (*stt_lenp > 0) {
      if (debug) {
	int i;
	fprintf(stderr, "SCSI request sense:\n");
	for (i=0; i<*stt_lenp; i++)
	  fprintf(stderr, " %02x", stt[i]);
	fprintf(stderr, "\n");
      }
    }
  }
  if (debug) fprintf(stderr, "retval = %d\n", retval);
  return(retval);
}


static int
scsi_reset(SCSI_HANDLE device, RESET_LEVEL level)
{
  int fd = GET_FD(device);
  int sg_scsi_reset;
  if (fd < 0) {
     if (debug) {
        fprintf(stderr, "aborting because fd = %d\n", fd);
     }
     return -1;
  }
  switch (level) {
  case RESET_DEVICE: sg_scsi_reset = SG_SCSI_RESET_DEVICE; break;
  case RESET_BUS   : sg_scsi_reset = SG_SCSI_RESET_BUS   ; break;
  }
  return ioctl(fd, SG_SCSI_RESET, &sg_scsi_reset);
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


