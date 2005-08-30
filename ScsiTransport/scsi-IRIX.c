/* -*- mode: c -*- */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>	/* for memset */


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
  int retval = 0;
  unsigned char *buf, *bp;
  struct sg_header *hdr;
  int fd;

  if (debug) {
    fprintf(stderr, "devfile = '%s'\n", devfile);
    fprintf(stderr, "cdb = 0x%.8lx, cdb_len =  %d\n", (long)cdb, cdb_len );
    fprintf(stderr, "dat = 0x%.8lx, dat_len =  %d\n", (long)dat,*dat_lenp);
    fprintf(stderr, "stt = 0x%.8lx, stt_len =  %d\n", (long)stt,*stt_lenp);
    fprintf(stderr, "timeout = %d\n", timeout);
  }
  if (direction == B_WRITE && debug) {
    int i;
    fprintf(stderr, "SCSI data to write:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }


  {
#include <dslib.h>
    static struct dsreq *dsp;
    int locmode;
    
    if ((dsp = dsopen(devfile, O_RDWR)) == NULL) {
      perror("open device");
      retval = -1;
      return retval;
    }
    
    locmode = DSRQ_DISC;
    if (direction == B_WRITE)
      locmode |= DSRQ_WRITE;
    if (direction == B_READ)
      locmode |= DSRQ_READ;
    
    if (*stt_lenp > 0) {
      locmode |= DSRQ_SENSE;
      memset(stt, 0, *stt_lenp);
    }
    /*fprintf(stderr, "stt = 0x%.8x, *stt_lenp = %d\n", stt, *stt_lenp);*/
    
    FLAGS(   dsp) = locmode;
    TIME(    dsp) = timeout*1000;
    CMDBUF(  dsp) = cdb;
    CMDLEN(  dsp) = cdb_len;
    DATABUF( dsp) = dat;
    DATALEN( dsp) =*dat_lenp;
    SENSEBUF(dsp) = stt;
    SENSELEN(dsp) =*stt_lenp;
    IOVLEN(  dsp) = 0;
    /* doscsireq(getfd(dsp), dsp); */
    ioctl(getfd(dsp), DS_ENTER, dsp);
    *dat_lenp = DATASENT( dsp);
    *stt_lenp = SENSESENT(dsp);
    retval    = STATUS(   dsp);
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


  if (direction == B_READ && debug && retval != -1) {
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
    retval = -1;
  } else {
    *stt_lenp = 0;
  }
  return(retval);
}

