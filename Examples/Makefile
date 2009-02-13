all:	tur inq dbgcnf

CFLAGS = -I../deliverables

tur:	tur.o
	$(CC) -o tur    tur.o    -L ../deliverables -lscsicmd

inq:	inq.o
	$(CC) -o inq    inq.o    -L ../deliverables -lscsicmd

dbgcnf:	dbgcnf.o
	$(CC) -o dbgcnf dbgcnf.o -L ../deliverables -lscsicmd

clean:
	rm -f *.o

clobber:	clean
	rm -f tur inq dbgcnf

