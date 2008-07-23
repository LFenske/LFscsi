#include <sys/types.h>	/* for socket, bind, htonl, htons, fork, waitpid */
#include <sys/socket.h>	/* for socket, bind */
#include <netdb.h>	/* for getprotobyname */
#include <netinet/in.h>	/* for htonl, htons */
#include <unistd.h>	/* for fork */
#include <stdlib.h>	/* for malloc */
#include <signal.h>	/* for signal */
#include <sys/wait.h>	/* for waitpid */
#include <errno.h>	/* for errno, EINTR */
#include <stdio.h>	/* for stderr */

#include <strings.h>
#include "ScsiTransport.h"


#define PORT 39074
#define QUEUESIZE 10


typedef unsigned char byte;


int use_children = 1;


static int
myread(int fd, void *buf, int nbyte)
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


void *
mymalloc(size_t size, char *errstring)
{
  void *retval;
  if (size) {
    retval = malloc(size);
    if (retval == NULL) {
      /* this should probably be a syslog instead of fprintf */
      fprintf(stderr, "malloc failed '%s'\n", errstring);
      exit(1);
    }
  } else {
    retval = NULL;
  }
  return retval;
}


void one_connection(int fd)
{
  for (;;) {
    byte devlen;
    byte *dev;
    SCSI_HANDLE handle;
    DIRECTION rw;
    byte cdblen;
    byte cdb[16];	/* assuming well-behaved client */
    int  datlen;
    byte *dat = NULL;
    int  sttlen;
    byte *stt;
    int  timout;
    byte status;
    long q;
    byte qc;

    if (!myread(fd, &devlen, 1)     ) break;
    dev = mymalloc(devlen+1, "dev");
    if (!myread(fd, dev    , devlen)) break; dev[devlen] = '\0';
    if (!myread(fd, &qc    , 1)     ) break;
    rw = (DIRECTION)qc;
    if (!myread(fd, &cdblen, 1)     ) break;
    if (!myread(fd, cdb    , cdblen)) break;
    if (!myread(fd, &datlen, 4)     ) break; datlen = ntohl(datlen);
    switch ((char)rw) {
      case DIRECTION_NONE: break;
      case DIRECTION_OUT:  break;
      case DIRECTION_IN:   break;
      case 'r': case 'R': rw = (datlen==0)?DIRECTION_NONE:DIRECTION_IN ; break;
      case 'w': case 'W': rw = (datlen==0)?DIRECTION_NONE:DIRECTION_OUT; break;
    }
    dat = mymalloc(datlen, "dat");
    if (rw == DIRECTION_OUT && dat != NULL) {
      if (!myread(fd, dat    , datlen)) break;
    }
    if (!myread(fd, &sttlen, 4)     ) break; sttlen = ntohl(sttlen);
    if (!myread(fd, &timout, 4)     ) break; timout = ntohl(timout);
    stt = mymalloc(sttlen, "stt");
    scsi_open(&handle, dev);
    status = (handle->cdb)(handle, rw, cdb, cdblen, dat, &datlen, stt, &sttlen, (float)timout);
    (handle->close)(&handle);
    /*debug*/fprintf(stderr, "server: datlen = %d\n", datlen);/**/
    if (rw != DIRECTION_IN) datlen = 0;
    write(fd, &status, 1);
    q = htonl(datlen);
    write(fd, &q     , 4); write(fd, dat, datlen);
    q = htonl(sttlen);
    write(fd, &q     , 4); write(fd, stt, sttlen);
    free(stt);
    free(dat);
    free(dev);
  }
  close(fd);
}


void
child_death(int p)
{
  waitpid(-1, NULL, WNOHANG);
  signal(SIGCHLD, child_death);
}


int
main()
{
  int S, NS;
  struct sockaddr_in srv;
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(addr);

  /* S = socket(AF_INET, SOCK_STREAM, (getprotobyname("tcp"))->p_proto); */
  S = socket(AF_INET, SOCK_STREAM, 0);
  if (S < 0) {
    perror("socket");
    exit(1);
  }

  srv.sin_family = AF_INET;
  srv.sin_addr.s_addr = htonl(INADDR_ANY);
  srv.sin_port = htons(PORT);
  if (bind(S, (struct sockaddr *) &srv, sizeof(srv)) < 0) {
    perror("bind");
    exit(1);
  }

  if (listen(S, QUEUESIZE) < 0) {
    perror("listen");
    exit(1);
  }

  signal(SIGCHLD, child_death);

  for (;;) {
    pid_t pid;

    for (;;) {
      NS = accept(S, (struct sockaddr *)&addr, &addr_len);
      if (NS >= 0) break;
      if (errno != EINTR) {
	perror("accept");
	exit(1);
      }
    }

    if (use_children) {
      if ((pid = fork())) {
	/* parent */
	close(NS);
      } else if (pid == (pid_t)-1) {
	/* fork failed */
	perror("fork");
	exit(1);
      } else {
	/* child */
	one_connection(NS);
	exit(0);
      }
    } else {
      one_connection(NS);
    }
  }

}
