CC = gcc
CWARN= -Wall
COPT = -O3
CINC = -I$(SCSITRANSPORT)
CFLAGS = $(CWARN) $(COPT) $(CINC)

SCSITRANSPORT   = ../ScsiTransport
SCSITRANSPORT_H = $(SCSITRANSPORT)/ScsiTransport.h

LIBS-IRIX  = -lds
LIBS-SunOS = -lsocket
LIBS-Linux =
LIBS-NetBSD= -lutil

OBJS-COMMON = scsiserver.o
OBJS-IRIX  = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-IRIX.o
OBJS-SunOS = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-SunOS.o
OBJS-Linux = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-Linux.o
OBJS-NetBSD= $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-NetBSD.o

all:
		$(MAKE) scsiserver-`uname -s | cut -d_ -f1`

.PHONY: $(SCSITRANSPORT)/scsi-IRIX.o
$(SCSITRANSPORT)/scsi-IRIX.o:
		cd $(SCSITRANSPORT); make
scsiserver-IRIX:	$(OBJS-IRIX)
		$(CC) -o $@ $^ $(LIBS-IRIX)

.PHONY: $(SCSITRANSPORT)/scsi-SunOS.o
$(SCSITRANSPORT)/scsi-SunOS.o:
		cd $(SCSITRANSPORT); make
scsiserver-SunOS:	$(OBJS-SunOS)
		$(CC) -o $@ $^ $(LIBS-SunOS)

.PHONY: $(SCSITRANSPORT)/scsi-Linux.o
$(SCSITRANSPORT)/scsi-Linux.o:
		cd $(SCSITRANSPORT); make
scsiserver-Linux:	$(OBJS-Linux)
		$(CC) -o $@ $^ $(LIBS-Linux)

.PHONY: $(SCSITRANSPORT)/scsi-NetBSD.o
$(SCSITRANSPORT)/scsi-NetBSD.o:
		cd $(SCSITRANSPORT); make
scsiserver-NetBSD:	$(OBJS-NetBSD)
		$(CC) -o $@ $^ $(LIBS-NetBSD)

scsiserver.o:	$(SCSITRANSPORT_H) scsiserver.c

clean:
		rm -f *.o

clobber:	clean
		rm -f scsiserver-* core *~

