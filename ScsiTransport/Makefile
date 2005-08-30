CC = gcc
CWARN= -Wall
COPT = -O3
#CINC = -Iamass-includes
CFLAGS = $(CWARN) $(COPT) $(CINC)

all:
		$(MAKE) scsi-`uname -s`.o

scsi-IRIX.o:		ScsiTransport.h scsi-IRIX.c
scsi-SunOS.o:		ScsiTransport.h scsi-SunOS.c
scsi-Linux.o:		ScsiTransport.h scsi-Linux.c
scsi-NetBSD.o:		ScsiTransport.h scsi-NetBSD.c

clean:
		rm -f *.o

clobber:	clean
		rm -f core *~

