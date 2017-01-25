#
# OCFLAGS:
# 	COUNT_IOS	- Counts struct io's left at end
# 	DEBUG		- Various and sundy debug asserts
# 	NDEBUG		- Defined: no asserts, Undefined: asserts
#

CC	= gcc
CFLAGS	= -Wall -W -g
INCS	= -I. -I.. -I../btt
OCFLAGS	= -UCOUNT_IOS 
XCFLAGS	= -D_GNU_SOURCE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
override CFLAGS += $(INCS) $(XCFLAGS) $(OCFLAGS)

PROGS	= btrecordG btreplay
LIBS	= -laio -lrt -lpthread

all: depend $(PROGS)

$(PROGS): | depend


clean:
	-rm -f *.o $(PROGS) .depend

%.o: %.c
	$(CC) $(CFLAGS) -c -o $*.o $<

btrecordG: btrecordG.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o,$^)

btreplay: btreplay.o
	$(CC) $(CFLAGS) -o $@ $(filter %.o,$^) $(LIBS)

depend:
	@$(CC) -MM $(CFLAGS) *.c 1> .depend

ifneq ($(wildcard .depend),)
include .depend
endif
