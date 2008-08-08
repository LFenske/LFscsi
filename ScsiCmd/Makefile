LIB = libscsicmd.a

CC = gcc
CWARN= -Wall
COPT = -O3
CINC = -I$(SCSITRANSPORT)
CFLAGS = $(CWARN) $(COPT) $(CINC)

SCSITRANSPORT = ../ScsiTransport
SCSITRANSPORT_H = $(SCSITRANSPORT)/ScsiTransport.h

Q=@

CSRCS =                 \
common.c                \
send_cdb.c              \
PrintDefaultSub.c       \
PrintInquirySub.c       \
PrintModeSenseSub.c     \
PrintReadPositionSub.c  \
PrintRequestSenseSub.c  \
CmdHelp.c               \
CmdInquiry.c            \
CmdLoadUnload.c         \
CmdLocate.c             \
CmdModeSelect.c         \
CmdModeSense.c          \
CmdPreventAllow.c       \
CmdRead.c               \
CmdRead6.c              \
CmdReadPosition.c       \
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
PrintReadPosition \
PrintRequestSense \
# this line intentionally left blank

MAINS =             \
scsi.o              \
PrintDefault.o      \
PrintInquiry.o      \
PrintModeSense.o    \
PrintReadPosition.o \
PrintRequestSense.o \
# this line intentionally left blank

all: allcommands.h $(LIB) $(PROGS)

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

.PHONY: clean
clean:
		-rm -f $(OBJS) $(MAINS) $(DEPS) allcommands.h *.map .\#*

.PHONY: clobber
clobber:	clean
		-rm -f $(LIB) $(PROGS) *~ core *.core TAGS

.PHONY: depend
depend:

.PHONE: TAGS
TAGS:
	emacs-etags *.[ch]*

../ScsiTransport/scsi.o:
		cd ../ScsiTransport; $(MAKE)

scsi:		scsi.o ../ScsiTransport/scsi.o $(LIB)
		gcc -o $@ $^

PrintDefault:		PrintDefault.o      $(LIB)
			gcc -o $@ $^

PrintInquiry:		PrintInquiry.o      $(LIB)
			gcc -o $@ $^

PrintModeSense:		PrintModeSense.o    $(LIB)
			gcc -o $@ $^

PrintReadPosition:	PrintReadPosition.o $(LIB)
			gcc -o $@ $^

PrintRequestSense:	PrintRequestSense.o $(LIB)
			gcc -o $@ $^

allcommands.h:	Makefile
		echo $(CSRCS) | tr " " "\012" | grep Cmd | while read i; do echo '#include "'$$i'"'; done > allcommands.h

