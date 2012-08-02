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
		mkdir -p ../deliverables/include
		cp -p ScsiTransport.h ../deliverables/include

clean:
		rm -f *.o

clobber:	clean
		rm -f core *~

