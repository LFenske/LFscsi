CC = gcc
CWARN= -Wall
COPT = -O3
CINC = -I$(SCSITRANSPORT)
CFLAGS = $(CWARN) $(COPT) $(CINC)

SCSITRANSPORT   = ../ScsiTransport
SCSITRANSPORT_H = $(SCSITRANSPORT)/ScsiTransport.h

LIBS-IRIX    = -lds
LIBS-SunOS   = -lsocket
LIBS-Linux   =
LIBS-NetBSD  = -lutil
LIBS-CYGWIN  =
LIBS-Template=

OBJS-COMMON = scsiserver.o
OBJS-IRIX    = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-IRIX.o
OBJS-SunOS   = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-SunOS.o
OBJS-Linux   = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-Linux.o
OBJS-NetBSD  = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-NetBSD.o
OBJS-CYGWIN  = $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-CYGWIN.o
OBJS-Template= $(OBJS-COMMON) $(SCSITRANSPORT)/scsi-Template.o

all:
		$(MAKE) scsiserver-`uname -s | cut -d_ -f1`

.PHONY: $(SCSITRANSPORT)/scsi-IRIX.o
$(SCSITRANSPORT)/scsi-IRIX.o:
		cd $(SCSITRANSPORT); $(MAKE)
scsiserver-IRIX:	$(OBJS-IRIX)
		$(CC) -o $@ $^ $(LIBS-IRIX)

.PHONY: $(SCSITRANSPORT)/scsi-SunOS.o
$(SCSITRANSPORT)/scsi-SunOS.o:
		cd $(SCSITRANSPORT); $(MAKE)
scsiserver-SunOS:	$(OBJS-SunOS)
		$(CC) -o $@ $^ $(LIBS-SunOS)

.PHONY: $(SCSITRANSPORT)/scsi-Linux.o
$(SCSITRANSPORT)/scsi-Linux.o:
		cd $(SCSITRANSPORT); $(MAKE)
scsiserver-Linux:	$(OBJS-Linux)
		$(CC) -o $@ $^ $(LIBS-Linux)

.PHONY: $(SCSITRANSPORT)/scsi-NetBSD.o
$(SCSITRANSPORT)/scsi-NetBSD.o:
		cd $(SCSITRANSPORT); $(MAKE)
scsiserver-NetBSD:	$(OBJS-NetBSD)
		$(CC) -o $@ $^ $(LIBS-NetBSD)

.PHONY: $(SCSITRANSPORT)/scsi-CYGWIN.o
$(SCSITRANSPORT)/scsi-CYGWIN.o:
		cd $(SCSITRANSPORT); $(MAKE)
scsiserver-CYGWIN:	$(OBJS-CYGWIN)
		$(CC) -o $@ $^ $(LIBS-CYGWIN)

.PHONY: $(SCSITRANSPORT)/scsi-Template.o
$(SCSITRANSPORT)/scsi-Template.o:
		cd $(SCSITRANSPORT); $(MAKE)
scsiserver-Template:	$(OBJS-Template)
		$(CC) -o $@ $^ $(LIBS-Template)

scsiserver.o:	$(SCSITRANSPORT_H) scsiserver.c

deliver:
# nothing, for now

clean:
		rm -f *.o

clobber:	clean
		rm -f scsiserver-* core *~

