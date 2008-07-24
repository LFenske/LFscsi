/* -*- mode: c -*- */

#include <stdio.h>
#include "/usr/src/linux/include/scsi/sg.h"
#include <fcntl.h>
#include <string.h>	/* for memset */
#include <sys/param.h>	/* for HZ */


#define debug 0

#define TIMEOUT_DEFAULT 30

#if 0
typedef char bool;
#define FALSE 0
#define TRUE (!FALSE)
#endif


#define B_WRITE 'w'
#define B_READ  'r'


#if 0
static int
usersub(ix, sp, items)
     int ix;
     int sp;
     int items;
{
  STR **st = stack->ary_array + sp;
  /*
  int i;
  char *tmps;
  STR *Str;
  */
  
  switch (ix) {
  case US_scsi:
    if (5 > items || items > 8 || items == 6)
      fatal("Usage: &scsi($devfile, $direction, $cdb, $dat, $dat_len, {$stt, $stt_len, {$timeout}})");
    else {
      int retval;
      char*	devfile =	(char*)		str_get (st[1]);
      char	direction =	(char)		str_gnum(st[2]);
      char*	cdb =		(char*)		str_get (st[3]);
      char*	dat =                           NULL;
      int	dat_len =	(int)		str_gnum(st[5]);
      char*	stt;
      int	stt_len;
      int	timeout;

      int	cdb_len;
      bool	dat_malloced = FALSE;

      if (items >= 7) stt_len = (int) str_gnum(st[7]);
      else            stt_len = 0;
      if (items >= 6 && stt_len) stt = (char*) malloc(stt_len);
      else            stt = NULL;
      if (items >= 8) timeout = (int) str_gnum(st[8]);
      else            timeout = TIMEOUT_DEFAULT;
      if (direction == B_WRITE && dat_len > 0) {
	dat = (char*) str_get(st[4]);
	dat_len = str_len(st[4]);
      }
      if (direction == B_READ && dat_len > 0) {
	dat = (char*) malloc(dat_len);
	dat_malloced = TRUE;
      }
      cdb_len = str_len(st[3]);

      retval = scsi(devfile, direction, cdb, cdb_len, dat, &dat_len, stt, &stt_len, timeout);

      if (direction == B_READ && dat_len >= 0)
	str_nset(st[4], (char*) dat, dat_len);
      if (items >= 6 && stt) {
	str_nset(st[6], (char*) stt, stt_len);
	free(stt);
      }
      str_numset(st[0], (double) retval);
      if (dat_malloced)
	free(dat);
    }
    return sp;
  default:	/* this can never happen */
    return -1;
  }
}
#endif


#if 0
static int
myread(fd, buf, nbyte)
     int fd;
     char *buf;
     int nbyte;
/* try harder to read nbytes into buf from fd */
/* only stop when there's nothing left to read or nbyte has been satisfied */
/* return number of bytes read */
{
  int curread;
  int bytesleft = nbyte;
  while (bytesleft) {
    curread = read(fd, buf, bytesleft);
    if (curread == 0)
      break;
    if (curread == -1)
      return -1;
    buf       += curread;
    bytesleft -= curread;
  }
  return nbyte-bytesleft;
}
#endif


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
    fd = open(devfile, O_RDWR);
    if (fd < 0) {
      perror("open device");
      retval = -1;
      return retval;
    }
    ioctl(fd, SG_SET_TIMEOUT, timeout*HZ);
    if (debug) fprintf(stderr, "timeout set to %d\n", timeout*HZ);
    bp = buf = (unsigned char *)malloc(sizeof(*hdr)+cdb_len+*dat_lenp);
    hdr = (struct sg_header *)buf;
    hdr->pack_len    = sizeof(*hdr) + ((direction==B_WRITE) ? *dat_lenp : 0) + cdb_len;
    hdr->reply_len   = sizeof(*hdr) + ((direction==B_READ ) ? *dat_lenp : 0);
    hdr->pack_id     = 0;
    hdr->result      = -1;
    hdr->twelve_byte = 0;
    hdr->other_flags = 0;
    memset(hdr->sense_buffer, 0, sizeof(hdr->sense_buffer));
    bp += sizeof(*hdr);
    memcpy(bp, cdb, cdb_len);
    bp += cdb_len;
    if (direction==B_WRITE)
      memcpy(bp, dat, *dat_lenp);
    if (debug) {
      int i;
      fprintf(stderr, "sizeof(header) = %d\n", sizeof(*hdr));
      fprintf(stderr, "buf out:\n");
      for (i=0; i<sizeof(*hdr); i++)
	fprintf(stderr, " %02x", buf[i]);
      fprintf(stderr, "\n");
      for (i=sizeof(*hdr); i<hdr->pack_len; i++)
	fprintf(stderr, " %02x", buf[i]);
      fprintf(stderr, "\n");
    }
    if (write(fd, buf, hdr->pack_len) == -1) {
      perror("sg_xfer(write)");
      retval = -1;
    }
    if (retval != -1) {
      if ((*dat_lenp=read(fd, buf, hdr->reply_len)) == -1) {
	perror("sg_xfer(read)");
	retval = -1;
      } else {
	*dat_lenp -= sizeof(*hdr);
	memcpy(dat, buf+sizeof(*hdr), *dat_lenp);
      }
    }
    if (hdr->result && retval != -1) {
      fprintf(stderr, "sg_xfer result = 0x%x\n", hdr->result);
      retval = -1;
    }
    close(fd);
  }

  
  if (direction == B_READ && debug && retval != -1) {
    int i;
    fprintf(stderr, "SCSI data read:\n");
    for (i=0; i<*dat_lenp; i++)
      fprintf(stderr, " %02x", dat[i]);
    fprintf(stderr, "\n");
  }

  if (hdr->sense_buffer[0] != 0 || retval == -1) {
    if (*stt_lenp > 0) {
      if (*stt_lenp > sizeof(hdr->sense_buffer))
	*stt_lenp = sizeof(hdr->sense_buffer);
      memcpy(stt, hdr->sense_buffer, *stt_lenp);
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

