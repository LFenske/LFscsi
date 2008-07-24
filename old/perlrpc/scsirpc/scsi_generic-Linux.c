/* -*- mode: c -*- */

#include <stdio.h>
#include "/usr/src/linux/include/scsi/sg.h"
#include <fcntl.h>
#include <string.h>	/* for memset */
#include <sys/param.h>	/* for HZ */
#include <sys/ioctl.h>	/* ioctl */
#include "scsi.h"


#define debug 0

#define TIMEOUT_DEFAULT 30

#define B_WRITE 'w'
#define B_READ  'r'


void get_memory(char **memory, int *allocated, int to_allocate);


send_cdb_retval *
send_cdb(send_cdb_params *params)
{
  static send_cdb_retval retval;
  static int dat_allocated=-1, stt_allocated=-1;
  unsigned char *obuf=NULL, *ibuf=NULL, *bp;
  struct sg_header *hdr;
  int fd;

  if (debug) {
    fprintf(stderr, "device  = '%s'\n", params->device);
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)params->cdb.cdb_val, params->cdb.cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)params->dat.dat_val, params->dat.dat_len);
    fprintf(stderr, "               dat_maxlen =  %d\n", params->dat_maxlen);
    fprintf(stderr, "               stt_maxlen =  %d\n", params->stt_maxlen);
    fprintf(stderr, "timeout = %d\n", params->timeout);
  }
  retval.status = 0;
  if (params->rw == B_WRITE && debug) {
    int i;
    fprintf(stderr, "SCSI data to write:\n");
    for (i=0; i<params->dat.dat_len; i++)
      fprintf(stderr, " %02x", params->dat.dat_val[i]);
    fprintf(stderr, "\n");
  }


  {
    fd = open(params->device, O_RDWR);
    if (fd < 0) {
      perror("open device");
      retval.status = -1;
      return &retval;
    }
    ioctl(fd, SG_SET_TIMEOUT, params->timeout*HZ);
    if (debug) fprintf(stderr, "timeout set to %d\n", params->timeout*HZ);
    bp = obuf = (unsigned char *)malloc(sizeof(*hdr)+params->cdb.cdb_len+params->dat.dat_len);
    hdr = (struct sg_header *)obuf;
    hdr->pack_len    = sizeof(*hdr) + params->dat.dat_len + params->cdb.cdb_len;
    hdr->reply_len   = sizeof(*hdr) + params->dat_maxlen;
    hdr->pack_id     = 0;
    hdr->result      = -1;
    hdr->twelve_byte = 0;
    hdr->other_flags = 0;
    memset(hdr->sense_buffer, 0, sizeof(hdr->sense_buffer));
    bp += sizeof(*hdr);
    memcpy(bp, params->cdb.cdb_val, params->cdb.cdb_len);
    bp += params->cdb.cdb_len;
    memcpy(bp, params->dat.dat_val, params->dat.dat_len);
    if (debug) {
      int i;
      fprintf(stderr, "sizeof(header) = %d\n", sizeof(*hdr));
      fprintf(stderr, "buf out:\n");
      for (i=0; i<sizeof(*hdr); i++)
	fprintf(stderr, " %02x", obuf[i]);
      fprintf(stderr, "\n");
      for (i=sizeof(*hdr); i<hdr->pack_len; i++)
	fprintf(stderr, " %02x", obuf[i]);
      fprintf(stderr, "\n");
    }
    if (write(fd, obuf, hdr->pack_len) == -1) {
      perror("sg_xfer(write)");
      retval.status = -1;
    }
    if (retval.status != -1) {
      ibuf = (unsigned char *)malloc(hdr->reply_len);
      if ((retval.dat.dat_len=read(fd, ibuf, hdr->reply_len)) == -1) {
	perror("sg_xfer(read)");
	retval.status = -1;
      } else {
	retval.dat.dat_len -= sizeof(*hdr);
	get_memory(&retval.dat.dat_val, &dat_allocated, retval.dat.dat_len);
	memcpy(retval.dat.dat_val, ibuf+sizeof(*hdr), retval.dat.dat_len);
      }
    }
    hdr = (struct sg_header *)ibuf;
    if (hdr->result && retval.status != -1) {
      fprintf(stderr, "sg_xfer result = 0x%x\n", hdr->result);
      retval.status = -1;
    }
    close(fd);
  }

  
  if (params->rw == B_READ && debug && retval.status != -1) {
    int i;
    fprintf(stderr, "SCSI data read:\n");
    for (i=0; i<retval.dat.dat_len; i++)
      fprintf(stderr, " %02x", (unsigned char)retval.dat.dat_val[i]);
    fprintf(stderr, "\n");
  }

  if (hdr->sense_buffer[0] != 0 || retval.status == -1) {
    retval.stt.stt_len = params->stt_maxlen;
    if (retval.stt.stt_len > 0) {
      if (retval.stt.stt_len > sizeof(hdr->sense_buffer))
	retval.stt.stt_len = sizeof(hdr->sense_buffer);
      get_memory(&retval.stt.stt_val, &stt_allocated, retval.stt.stt_len);
      memcpy(retval.stt.stt_val, hdr->sense_buffer, retval.stt.stt_len);
      if (debug) {
	int i;
	fprintf(stderr, "SCSI request sense:\n");
	for (i=0; i<retval.stt.stt_len; i++)
	  fprintf(stderr, " %02x", (unsigned char)retval.stt.stt_val[i]);
	fprintf(stderr, "\n");
      }
    }
    retval.status = -1;
  } else {
    retval.stt.stt_len = 0;
  }
  if (NULL != obuf) free(obuf);
  if (NULL != ibuf) free(ibuf);
  return &retval;
}


void
get_memory(char **memory, int *allocated, int to_allocate)
{
  if (*allocated < to_allocate) {
    if (*allocated >= 0)
      free(*memory);
    *memory = malloc(to_allocate);
    *allocated = to_allocate;
  }
}



