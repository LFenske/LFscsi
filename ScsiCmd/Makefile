LIB = libscsicmd.a
INC =    scsicmd.h

CC = gcc
CWARN= -Wall -Werror
COPT = -O3
CINC = -I$(SCSITRANSPORT)
CFLAGS = $(CWARN) $(COPT) $(CINC)
LNFLAGS = -static

SCSITRANSPORT = ../ScsiTransport
SCSITRANSPORT_H = $(SCSITRANSPORT)/ScsiTransport.h

Q=@

CSRCS =                 \
common.c                \
send_cdb.c              \
PrintDefaultSub.c       \
PrintInquirySub.c       \
PrintModeSenseSub.c     \
PrintReadCapacitySub.c  \
PrintReadPositionSub.c  \
PrintRequestSenseSub.c  \
CmdCDB.c                \
CmdHelp.c               \
CmdInquiry.c            \
CmdLoadUnload.c         \
CmdLocate.c             \
CmdModeSelect.c         \
CmdModeSense.c          \
CmdPreventAllow.c       \
CmdRead.c               \
CmdRead6.c              \
CmdReadCapacity.c       \
CmdReadPosition.c       \
CmdReportLuns.c         \
CmdRequestSense.c       \
CmdReset.c              \
CmdRewind.c             \
CmdSpace.c              \
CmdTestUnitReady.c      \
CmdWrite6.c             \
CmdWriteFilemarks.c     \
#CmdTemplate.c           \
# this line intentionally left blank

PROGS =           \
scsi              \
PrintDefault      \
PrintInquiry      \
PrintModeSense    \
PrintReadCapacity \
PrintReadPosition \
PrintRequestSense \
# this line intentionally left blank

MAINS =             \
scsi.o              \
PrintDefault.o      \
PrintInquiry.o      \
PrintModeSense.o    \
PrintReadCapacity.o \
PrintReadPosition.o \
PrintRequestSense.o \
# this line intentionally left blank

DELIVERABLES = $(INC) common.h send_cdb.h $(PROGS)

all: $(INC) $(LIB) $(PROGS)

OBJS = $(CSRCS:.c=.o)
DEPS = $(CSRCS:.c=.d) $(MAINS:.o=.d)

Cmd%.d:	Cmd%.c
	$(Q)echo building dependencies for $<; set -e; $(CC) -MM $(WFLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -DCOMMAND $< | sed 's@\(.*\)\.o[ :]*@Cmd$*.o $@ : @' > $@; [ -s $@ ] || rm -f $@

Cmd%.o:	Cmd%.c
	$(Q)echo compiling $<;                 set -e; $(CC)     $(WFLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) -DCOMMAND $< -o $@ -c

%.d:	%.c
	$(Q)echo building dependencies for $<; set -e; $(CC) -MM $(WFLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) $< | sed 's@\(.*\)\.o[ :]*@$*.o $@ : @' > $@; [ -s $@ ] || rm -f $@

%.o:	%.c
	$(Q)echo compiling $<;                 set -e; $(CC)     $(WFLAGS) $(CFLAGS) $(INCLUDES) $(DEFINES) $< -o $@ -c

$(LIB):	$(OBJS)
	$(Q)echo building library $(LIB)
	$(Q)rm -f $(LIB)
	$(Q)$(AR) qc $(LIB) $(OBJS)

ifneq ($(DEPS),)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),clobber)
ifneq ($(MAKECMDGOALS),scratch)
include $(DEPS)
endif
endif
endif
endif

.PHONY: deliver
deliver:	$(DELIVERABLES)
		mkdir -p ../deliverables
		cp -p $(DELIVERABLES) ../deliverables

.PHONY: clean
clean:
		-rm -f $(OBJS) $(MAINS) $(DEPS) allcommands.h *.map .\#*

.PHONY: clobber
clobber:	clean
		-rm -f $(LIB) $(INC) $(PROGS) *~ core *.core TAGS

.PHONY: depend
depend:

.PHONE: TAGS
TAGS:
	emacs-etags *.[ch]*

$(SCSITRANSPORT)/scsitransport.o:
		cd $(SCSITRANSPORT); $(MAKE)

scsi:		scsi.o $(SCSITRANSPORT)/scsitransport.o $(LIB)
		$(CC) $(LNFLAGS) -o $@ $^

scsi.o:		scsi.c allcommands.h

PrintDefault:		PrintDefault.o      $(LIB)
			$(CC) $(LNFLAGS) -o $@ $^

PrintInquiry:		PrintInquiry.o      $(LIB)
			$(CC) $(LNFLAGS) -o $@ $^

PrintModeSense:		PrintModeSense.o    $(LIB)
			$(CC) $(LNFLAGS) -o $@ $^

PrintReadCapacity:	PrintReadCapacity.o $(LIB)
			$(CC) $(LNFLAGS) -o $@ $^

PrintReadPosition:	PrintReadPosition.o $(LIB)
			$(CC) $(LNFLAGS) -o $@ $^

PrintRequestSense:	PrintRequestSense.o $(LIB)
			$(CC) $(LNFLAGS) -o $@ $^

allcommands.h:	Makefile
		echo $(CSRCS) | tr " " "\012" | grep Cmd | while read i; do echo '#include "'$$i'"'; done > allcommands.h

scsicmd.h:	Makefile
		echo $(CSRCS:.c=.h) | tr " " "\012" | grep -e Cmd -e Print | while read i; do cat $$i; done > scsicmd.h

