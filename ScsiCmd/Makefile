LIB = libscsicmd.a

CC = gcc
CWARN= -Wall
COPT = -O3
CINC = -I$(SCSITRANSPORT)
CFLAGS = $(CWARN) $(COPT) $(CINC)

SCSITRANSPORT = ../ScsiTransport
SCSITRANSPORT_H = $(SCSITRANSPORT)/ScsiTransport.h

Q=@

all: $(LIB)

CSRCS = \
CmdInquiry.c \
#

OBJS = $(CSRCS:.c=.o)
DEPS = $(CSRCS:.c=.d) $(MAINS:.o=.d)

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
		-rm -f $(OBJS) $(DEPS) *.map .\#*

.PHONY: clobber
clobber:	clean
		-rm -f $(LIB) *~ core *.core TAGS

.PHONY: depend
depend:

.PHONE: TAGS
TAGS:
	emacs-etags *.[ch]*
