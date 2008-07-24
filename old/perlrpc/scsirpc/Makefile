CC = gcc
COPT = -O3
CINC = -I. -Iamass-includes
CFLAGS = $(COPT) $(CINC) -Wall

LIBS-IRIX  = -lds
LIBS-SunOS = -lsocket
LIBS-Linux =

BIN = scsi_client scsi_server-IRIX scsi_server-SunOS scsi_server-Linux
GEN = scsi_clnt.c scsi_svc.c scsi_xdr.c scsi.h
LIB =
RPCCOM = rpcgen
OBJS-COMMON = scsi_server.o scsi_xdr.o scsi_svc.o
OBJS-IRIX   = $(OBJS-COMMON) scsi_generic-IRIX.o
OBJS-SunOS  = $(OBJS-COMMON) scsi_generic-SunOS.o
OBJS-Linux  = $(OBJS-COMMON) scsi_generic-Linux.o
OBJS-CLIENT = scsi_client.o scsi_xdr.o scsi_clnt.o

all:		scsi_client
		make scsi_server-`uname -s`

scsi_clnt.c scsi_svc.c scsi_xdr.c scsi.h:	scsi.x #scsi_svc.patch
		$(RPCCOM) scsi.x
		patch < scsi_svc-`uname -s`.patch

scsi_client:	$(OBJS-CLIENT)
		$(CC) $(CFLAGS) -o $@ $^

scsi_server-IRIX:	$(OBJS-IRIX)
		$(CC) $(CFLAGS) -o $@ $^  $(LIBS-IRIX)

scsi_server-SunOS:	$(OBJS-SunOS)
		$(CC) $(CFLAGS) -o $@ $^ $(LIBS-SunOS)

scsi_server-Linux:	$(OBJS-Linux)
		$(CC) $(CFLAGS) -o $@ $^ $(LIBS-Linux)

scsi_client.o:		scsi_client.c                       scsi.h
scsi_server.o:		scsi_server.c        scsi_generic.h scsi.h
scsi_generic-IRIX.o:	scsi_generic-IRIX.c  scsi_generic.h scsi.h
scsi_generic-SunOS.o:	scsi_generic-SunOS.c scsi_generic.h scsi.h
scsi_generic-Linux.o:	scsi_generic-Linux.c scsi_generic.h scsi.h

clean:
		rm -f $(GEN) *.o

clobber:	clean
		rm -f $(BIN)

