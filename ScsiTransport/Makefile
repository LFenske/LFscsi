CC = gcc
CWARN= -Wall
COPT = -O3
#CINC = -Iamass-includes
CFLAGS = $(CWARN) $(COPT) $(CINC)

all:	scsitransport.o
scsitransport.o:
		$(MAKE) scsi-`uname -s | cut -d_ -f1`.o
		ln -f scsi-`uname -s | cut -d_ -f1`.o scsitransport.o

scsi-IRIX.o:		ScsiTransport.h scsi-IRIX.c
scsi-SunOS.o:		ScsiTransport.h scsi-SunOS.c
scsi-Linux.o:		ScsiTransport.h scsi-Linux.c
scsi-NetBSD.o:		ScsiTransport.h scsi-NetBSD.c
scsi-CYGWIN.o:		ScsiTransport.h scsi-CYGWIN.c
scsi-Template.o:	ScsiTransport.h scsi-Template.c

deliver:
		cp -p ScsiTransport.h ../deliverables

clean:
		rm -f *.o

clobber:	clean
		rm -f core *~

