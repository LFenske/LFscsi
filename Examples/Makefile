all:	tur inq dbgcnf tur-raw

CFLAGS = -I../deliverables/include

tur:	tur.o
	$(CC) -o tur     tur.o     -L ../deliverables/lib -lscsicmd

inq:	inq.o
	$(CC) -o inq     inq.o     -L ../deliverables/lib -lscsicmd

dbgcnf:	dbgcnf.o
	$(CC) -o dbgcnf  dbgcnf.o  -L ../deliverables/lib -lscsicmd

tur-raw:tur-raw.o
	$(CC) -o tur-raw tur-raw.o -L ../deliverables/lib -lscsicmd

clean:
	rm -f *.o *~

clobber:	clean
	rm -f tur inq dbgcnf tur-raw

