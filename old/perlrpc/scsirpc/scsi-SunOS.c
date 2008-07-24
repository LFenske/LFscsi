/* -*- mode: c -*- */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>	/* for memset */
#include <errno.h>	/* for errno */
#include <sys/types.h>	/* for stat */
#include <sys/stat.h>	/* for stat */


#define debug 0

#define TIMEOUT_DEFAULT 30

#define B_WRITE 'w'
#define B_READ  'r'

#define FALSE 0
#define TRUE (!FALSE)


#if defined(sun) && defined(__SVR4)	/* only works with gcc */
/* Solaris */

#define Solaris2
#define SVR4
#define scsi_debug 0
#define LOG_E LOG_ERR
#include <sys/scsi/impl/uscsi.h>
#include <syslog.h>
#include <vd/scsilib.h>

int scsitype;


/* set the global scsitype int for sq or cf driver */
int
set_scsi_type()
{
	struct stat statbuf;

#if defined (sun) && !defined (Solaris2)
	if ((stat("/dev/sq", &statbuf)) == 0)  {
		/* check for utter confusion */	
		if ((stat("/dev/cf", &statbuf)) == 0)  {
			/* both drivers defined...fatal error */
			syslog(LOG_S, "<AMASS_S_0129> Both scsi drivers, /dev/sq and /dev/cf are defined...Only one driver type should be defined");
			return(-1);
		}
		/* using sq driver */
		scsitype = 1;
		return(0);
	} else if ((stat("/dev/cf", &statbuf)) == 0)  {
		/* using cf driver */
		scsitype = 2;
		return(0);
	}
	/* no scsi type returned...fatal error */
	syslog(LOG_S, "<AMASS_S_0130> Neither scsi driver, /dev/sq or /dev/cf is defined");
	return(-1);
#else /* !(sun) || (Solaris2) */
	/* everyone non-sun (including Solaris) sets scsitype = 1 */
	scsitype = 1;
	return(0);
#endif /* !(sun) || (Solaris2) */
}

/* this does what get_sense used to do: read the sense info
 * from the hardware (or driver).  this routine is called
 * immediately after an error is detected, the result to be
 * saved away until a later call to get_sense (which moved to
 * scsilib.c).  
 */
int
extract_sense(devfd, snseptr)
int devfd;	/* scsi device file descr. */
register SENSEINFO *snseptr;
{

#if defined (sun) && !defined (SVR4)
	struct getsense sensedata; /* matches size of scsi_ext_sense */
 
	if (scsitype == 1) {  /* sq driver */
		if (ioctl(devfd, GETSENSE, &sensedata) < 0) {
			syslog(LOG_E, "<AMASS_E_0117> extract_sense: GETSENSE failed - errno = %d - %m\n", errno);
			snseptr->sense_valid = 0;
			return(-1);
		}

		snseptr->sense_valid = 1;
		snseptr->sense_error = sensedata.gsense[0] & 0x7f;
        snseptr->sense_fm = (sensedata.gsense[2] & 0x80) >> 7;
        snseptr->sense_eom = (sensedata.gsense[2] & 0x40) >> 6;
        snseptr->sense_ili = (sensedata.gsense[2] & 0x20) >> 5;
		snseptr->sense_key = sensedata.gsense[2] & 0x0f;
		snseptr->sense_code = sensedata.gsense[12];
		snseptr->sense_qual = sensedata.gsense[13];
		snseptr->sense_vendor = sensedata.gsense[18];
		bcopy(&sensedata.gsense[0], &snseptr->sense_bytes[0], 
				sizeof(sensedata.gsense));
	} else {  /* must be cf */
		syslog(LOG_S,"<AMASS_S_0148> Request sense for ciprico not supported");
		return(-1);
	}
	return(0);
#endif /* (sun) && !(SVR4) */

#if defined (IRIX)
    u_char *buf;
    struct dsreq *dsp;

    dsp = getdsp(devfd);
    buf = SENSEBUF(dsp);

	snseptr->sense_valid = 1;
    snseptr->sense_error = buf[0] & 0x7f;
    snseptr->sense_fm = (buf[2] & 0x80) >> 7;
    snseptr->sense_eom = (buf[2] & 0x40) >> 6;
    snseptr->sense_ili = (buf[2] & 0x20) >> 5;
    snseptr->sense_key =  buf[2] & 0x0f;
        snseptr->sense_code = buf[12];
        snseptr->sense_qual = buf[13];
        snseptr->sense_vendor = buf[18];
	bcopy(&buf[0], &snseptr->sense_bytes[0], 128);
    return(0);
#endif /* IRIX */

#if defined (SVR4) && !defined(IRIX)
{
	struct scdb generic_cdb;
	register struct scdb *cp;
	register int cplen;
	u_char buf[128];
	int i=0;


	cp = &generic_cdb;
	cplen = 6;
	memset((void *)cp, 0, cplen);
	cp->cmd = 0x03;
	cp->byte4 = (u_char)sizeof(buf);
	if( exec_scsi_command( devfd, cp, cplen, (caddr_t)buf, 
				sizeof(buf), 30, (u_short)SCB_READ )) {
			syslog(LOG_E, "<AMASS_E_0117> Request sense failed - errno = %d - %m\n", errno);
			snseptr->sense_valid = 0;
			return(-1);
	}
	snseptr->sense_valid = 1;
	snseptr->sense_error = buf[0] & 0x7f;
    snseptr->sense_fm  = (u_char)((u_char)(buf[2] & 0x80) >> 7);
    snseptr->sense_eom = (u_char)((u_char)(buf[2] & 0x40) >> 6);
    snseptr->sense_ili = (u_char)((u_char)(buf[2] & 0x20) >> 5);
	snseptr->sense_key =  buf[2] & 0x0f;
		snseptr->sense_code = buf[12];
		snseptr->sense_qual = buf[13];
		snseptr->sense_vendor = buf[18];
	memcpy(&snseptr->sense_bytes[0], &buf[0], 128);
}
	return(0);
#endif /* (SVR4) */

#ifdef AIX
{
	/* the scpass driver thinks this structure looks different
	 * however the length is the same and it just stuffs bytes
	 * in so we do it this way for standardization.
	 */
	struct getsense sensedata; /* matches size of scsi_ext_sense */


	if( ioctl( devfd, SCPASS_REQUEST_SENSE, &sensedata) == -1) {
			syslog(LOG_S, "<AMASS_E_0117> Request sense failed - errno = %d - %m\n", errno);
			snseptr->sense_valid = 0;
			return(-1);
	}
 
	snseptr->sense_valid = 1;
    snseptr->sense_error = sensedata.gsense[0] & 0x7f;
    snseptr->sense_fm = (sensedata.gsense[2] & 0x80) >> 7;
    snseptr->sense_eom = (sensedata.gsense[2] & 0x40) >> 6;
    snseptr->sense_ili = (sensedata.gsense[2] & 0x20) >> 5;
	snseptr->sense_key = sensedata.gsense[2] & 0x0f;
		snseptr->sense_code = sensedata.gsense[12];
		snseptr->sense_qual = sensedata.gsense[13];
		snseptr->sense_vendor = sensedata.gsense[18];
	bcopy(&sensedata.gsense[0], &snseptr->sense_bytes[0], 
			sizeof(sensedata.gsense));

}
	return(0);
#endif /* AIX */

#ifdef hpux807
{
	struct sense_2 sense_2;

	if(ioctl(devfd, SIOC_XSENSE, &sense_2)<0){
		syslog(LOG_S, "<AMASS_E_0117> Request sense failed - errno = %d - %m\n", errno);
		snseptr->sense_valid = 0;
		return(-1);
	}
	snseptr->sense_valid = 1;
	snseptr->sense_error = sense_2.error_code;
	snseptr->sense_key = sense_2.key;
	snseptr->sense_code = sense_2.code;
	snseptr->sense_qual = sense_2.qualifier;
}
	return(0);
#endif /* hpux807 */
}

void
dump_debug_data(pptr, len, name)
	char *pptr;
	int len;
	char *name;
 
{
	int i = 0, max = 128;
	char *addr, ptr[384];

	if( len <= 0 )
		return;
	
	if( len > max )
		len = max;

	addr = pptr;

	for (i=0; i<len; i++) {
		sprintf(&ptr[i*3], " %02x", (int)(*addr++ & 0xff));
	}
 
	ptr[i*3] = '\0';
 
	syslog(LOG_E,"%s:  %s\n",name,ptr);
	    	
}

static struct sensesave {
	int fd;
	SENSEINFO senseinfo;
	unsigned passthru_result;
	unsigned sense_count;
	unsigned char sense_bytes[128];
	struct sensesave *next;
} *sshead = NULL;

struct sensesave *
findsense(fd)
int fd;
{
	struct sensesave *ss;

	for(ss=sshead;; ss=ss->next){
		if(!ss){
			if ( (ss = (struct sensesave *) malloc(sizeof(*ss))) == NULL )
				return(NULL);
			memset((void *)&ss->senseinfo, 0, sizeof(ss->senseinfo));
			ss->fd=fd;
			ss->next=sshead;
			sshead=ss;
			break;
		}
		if(ss->fd==fd)
			break;
	}
	return(ss);
}

/* Read and save current senseinfo, per fd.
 * Should only be called after an error.
 * Subsequent calls to get_sense will return the
 * senseinfo saved here.
 */
int
savesense(fd)
int fd;
{
	struct sensesave *ss;

	if ( (ss = findsense(fd)) == NULL )
		return(-1);
	return extract_sense(fd, &ss->senseinfo);
}

int
exec_scsi_command( fd, cdbp, cdblen, data, datasize, timeout, mode )
int fd;  
register u_char *cdbp;    /* passed-in cdb */
register u_char cdblen;     /* passed-in cdb length */
caddr_t data;       /* data to r/w */
int datasize;
int timeout;        /* in seconds */
u_short mode;  /* read/write/no-xfer mode */
{
	struct uscsi_cmd cmd;
	register struct uscsi_cmd *passp = &cmd;
	int	err, retries;
	char    rqbuf[14];
		 
	/* timeout = set_scsi_timeout(timeout, ((struct scdb *)cdbp)->cmd); */
	retries =0;
	if (scsitype == 1) { /* sq driver */
		/* create the scsi command from parameters */
		memset((void *)passp, 0, sizeof (struct uscsi_cmd));
		passp->uscsi_timeout = timeout;
		passp->uscsi_cdb = (caddr_t)cdbp;
		passp->uscsi_cdblen = cdblen;
		passp->uscsi_bufaddr = data;
		if ((passp->uscsi_cdb[0] == 0x3) && (datasize == 0))
			passp->uscsi_buflen = datasize = 32;
		else
			passp->uscsi_buflen = datasize;

		if (datasize) {
			if (mode & SCB_READ)
				passp->uscsi_flags = USCSI_READ;
			else
				passp->uscsi_flags = USCSI_WRITE;
		}
																						if (mode & SCB_SILENT)
			passp->uscsi_flags |= USCSI_SILENT;
		if (mode & SCB_NORETRY)
		  	;  /* no equivalent flag */

sunretries:
                if (scsi_debug){
                    dump_debug_data((char *)passp->uscsi_cdb, (int)passp->uscsi_cdblen,"CDB");
                }

		if ((err = ioctl(fd, USCSICMD, passp)) < 0) {
			if (errno == EBUSY) {
				if (++retries < 5) {
					sleep (2);
					goto sunretries;
				}
			}
			if( !(mode & SCB_SILENT) )
			  syslog(LOG_DEBUG,"SCSI command 0x%x failed: %m\n", 
								*cdbp);

			if (scsi_debug){
			   syslog(LOG_E,"%m\n",errno);
				if( passp->uscsi_flags & USCSI_WRITE )
					dump_debug_data((char *)passp->uscsi_bufaddr,(int)passp->uscsi_buflen,"DATA");
			   /* fake sense */
			   /* if ioctl returned with -1 I don't expect sense */
			   /* however check for uscsi_status */
			   if (passp->uscsi_status != 02 ) {
				passp->uscsi_rqlen = 14;
				passp->uscsi_rqbuf   = &rqbuf[0];
				passp->uscsi_rqbuf[2] = 0xff;
			   }
			}
			/* sensing, return */
			if ( cdbp[0] == 0x03 ) {
				return(-1);
			}
			
			/* save sense if available */
			if (passp->uscsi_status == 02) 
			     savesense(fd);
		        else {
		             passp->uscsi_rqlen = 14;
			     passp->uscsi_rqbuf   = &rqbuf[0];
			     passp->uscsi_rqbuf[2] = 0xff;  
	                     savesense(fd);
			}			
			dump_debug_data((char *)passp->uscsi_rqbuf,
			    (int)passp->uscsi_rqlen,"SENSE");
			return(-1);
		}
		else {
        		if (scsi_debug){
				dump_debug_data((char *)passp->uscsi_bufaddr,(int)passp->uscsi_buflen,"DATA");
			}
		}
	}
	else { /* ciprico */
	  	syslog(LOG_E,"pass thru ciprico not implemented\n");
		return(-1);
	}

	return(0);
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
#include <vd/scsilib.h>
    static int opened = FALSE;
    static int fd, dsp;
    int ret;
    char *sensep;
    struct SENSESAVE {
      int fd;
      SENSEINFO senseinfo;
      unsigned passthru_result;
      unsigned sense_count;
      unsigned char sense_bytes[128];
      struct sensesave *next;
    } *SaveSense;
    
    if (!opened) {

      if (set_scsi_type() < 0)
	exit(-1);

#if defined(hpux)
      if ((fd = open(devfile, O_RDWR)) < 0) {
	perror("open device");
	exit(-1);
      }
#elif defined(IRIX)
      if ((dsp = dsopen(devfile, O_RDWR)) == NULL) {
	perror("open device");
	exit(-1);
      }
      fd = addfd(dsp);
#else
      if ((fd = open(devfile, O_NOCTTY)) < 0) {
	perror("open device");
	exit(-1);
      }
#endif

      opened = TRUE;
    }

    /** /fprintf(stderr, "exec_scsi_command(%d, (%d,%d,%d,%d,%d,%d), %d, dat, %d, %d, %d)\n", fd, cdb[0], cdb[1], cdb[2], cdb[3], cdb[4], cdb[5], cdb_len, *dat_lenp, timeout, direction == B_WRITE ? SCB_WRITE : dat_len == 0 ? SCB_NORETRY : SCB_READ);/**/
    ret = exec_scsi_command(fd,
			    cdb, cdb_len,
			    dat,*dat_lenp,
			    timeout,
			    direction == B_WRITE ? SCB_WRITE :
			    *dat_lenp == 0 ? SCB_NORETRY : SCB_READ);

    if (ret < 0) {
      if (stt && *stt_lenp) {
	SaveSense = (struct SENSESAVE *)findsense(fd);
	
#ifdef hpux
	sensep = SaveSense->sense_bytes;
#else
	sensep = SaveSense->senseinfo.sense_bytes;
#endif
	
	if (*stt_lenp > sensep[7]+8)
	  *stt_lenp = sensep[7]+8;
	
	memcpy(stt, sensep, *stt_lenp);
      }
      else *stt_lenp = 0;
      retval = 2;
    }
    else *stt_lenp = 0;
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


