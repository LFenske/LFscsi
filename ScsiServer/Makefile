# Copyright (C) 2008  Larry Fenske
# 
# This file is part of LFscsi.
# 
# LFscsi is free software: you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# LFscsi is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with LFscsi.  If not, see <http://www.gnu.org/licenses/>.

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

