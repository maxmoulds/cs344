CFLAGS = -fpic -lm -lpthread

test:
	gcc -o btest test.c $(CFLAGS)

all:
	gcc -o buildrooms mouldsm.buildrooms.c $(CFLAGS)	
		


clean:
	rm -f btest
	rm -f buildrooms
	rm -rf *.rooms.*
	rm -f *.gch
