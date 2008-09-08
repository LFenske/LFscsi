all:	tur

CFLAGS = -I../deliverables

tur:	tur.o
	gcc -o tur tur.o -L ../deliverables -lscsicmd

clean:
	rm -f *.o

clobber:	clean
	rm -f tur

