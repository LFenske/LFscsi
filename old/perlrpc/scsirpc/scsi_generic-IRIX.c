/* -*- mode: c -*- */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>	/* for memset */
#include <sys/param.h>	/* for HZ */
#include <sys/ioctl.h>	/* ioctl */
#include "scsi.h"


#define debug 1

#define TIMEOUT_DEFAULT 30

#define B_WRITE 'w'
#define B_READ  'r'


void get_memory(char **memory, int *allocated, int to_allocate);


send_cdb_retval *
send_cdb(send_cdb_params *params)
{
  static send_cdb_retval retval;
  static int dat_allocated=-1, stt_allocated=-1;

  if (debug) {
    fprintf(stderr, "device  = '%s'\n", params->device);
    fprintf(stderr, "rw  = '%c'\n", params->rw);
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
#include <dslib.h>
    static struct dsreq *dsp;
    int locmode;
    
    if ((dsp = dsopen(params->device, O_RDWR)) == NULL) {
      perror("open device");
      retval.status = -1;
      return &retval;
    }

    locmode = DSRQ_DISC;
    if (params->rw == B_WRITE)
      locmode |= DSRQ_WRITE;
    if (params->rw == B_READ)
      locmode |= DSRQ_READ;

    if (params->stt_maxlen > 0) {
      locmode |= DSRQ_SENSE;
      get_memory(&retval.stt.stt_val, &stt_allocated, params->stt_maxlen);
      memset(retval.stt.stt_val, 0, params->stt_maxlen);
    }

    FLAGS(   dsp) = locmode;
    TIME(    dsp) = params->timeout*1000;
    CMDBUF(  dsp) = params->cdb.cdb_val;
    CMDLEN(  dsp) = params->cdb.cdb_len;
    if (params->rw == B_READ) {
      get_memory(&retval.dat.dat_val, &dat_allocated, params->dat_maxlen);
      DATABUF( dsp) = retval.dat.dat_val;
      DATALEN( dsp) = params->dat_maxlen;
    } else {
      DATABUF( dsp) = params->dat.dat_val;
      DATALEN( dsp) = params->dat.dat_len;
    }
    SENSEBUF(dsp) = retval.stt.stt_val;
    SENSELEN(dsp) = params->stt_maxlen;
    IOVLEN(  dsp) = 0;
    /* doscsireq(getfd(dsp), dsp); */
    ioctl(getfd(dsp), DS_ENTER, dsp);
    retval.dat.dat_len = DATASENT( dsp);
    retval.stt.stt_len = SENSESENT(dsp);
    retval.status      = STATUS(   dsp);
    dsclose(dsp);

    /*
      {
      dsconf_t conf;
      int i;
      long dsget;
      fprintf(stderr, "DS_GET = %d\n", ioctl(getfd(dsp), DS_GET, &dsget));
      fprintf(stderr, "ds_get: 0x%.8x = %d\n", dsget, dsget);
      dsget = 0xff;
      fprintf(stderr, "DS_SET = %d\n", ioctl(getfd(dsp), DS_SET, &dsget));
      perror("DS_SET");
      fprintf(stderr, "DS_GET = %d\n", ioctl(getfd(dsp), DS_GET, &dsget));
      fprintf(stderr, "ds_get: 0x%.8x = %d\n", dsget, dsget);
      ioctl(getfd(dsp), DS_CONF, &conf);
      for (i=0; i<sizeof(conf)/4; i++)
      fprintf(stderr, "%d: 0x%.8x = %d\n", i, ((long*)(&conf))[i], ((long*)(&conf))[i]);
      }
      */
    
  }

  
  if (params->rw == B_READ && debug && retval.status != (char)(-1)) {
    int i;
    fprintf(stderr, "SCSI data read:\n");
    for (i=0; i<retval.dat.dat_len; i++)
      fprintf(stderr, " %02x", (unsigned char)retval.dat.dat_val[i]);
    fprintf(stderr, "\n");
  }
  
  if (retval.stt.stt_val[0] != 0 || retval.status == (char)(-1)) {
    if (retval.stt.stt_len > 0) {
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
  if (debug) {
    fprintf(stderr, "                   status =  %d\n", retval.status);
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)retval.dat.dat_val, retval.dat.dat_len);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)retval.stt.stt_val, retval.stt.stt_len );
  }
  return &retval;
}


void
get_memory(char **memory, int *allocated, int to_allocate)
{
  if (debug) fprintf(stderr, "get_memory in  0x%.8x (*=0x%.8x) 0x%x (*=%d) %d\n", memory, *memory, allocated, *allocated, to_allocate);
  if (*allocated < to_allocate) {
    if (*allocated >= 0)
      free(*memory);
    *memory = malloc(to_allocate);
    *allocated = to_allocate;
  }
  if (debug) fprintf(stderr, "get_memory out 0x%.8x (*=0x%.8x) 0x%x (*=%d) %d\n", memory, *memory, allocated, *allocated, to_allocate);
}


