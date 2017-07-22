CFLAGS = -fpic -lm -lpthread

unittestresults.out: all
	rm -f unittestresults.out
	$(foreach a, $(Tests), gcc -o testing asserttrue.c $(a) -g dominion.o rngs.o $(CFLAGS) ; \
	./testing &> unittestresults.out ;)
	gcov dominion.c >> unittestresults.out
	cat dominion.c.gcov >> unittestresults.out
	echo $(Tests)

#Tests = cardtest4.c

Tests:=$(shell ls  | egrep '*test[0-9]+.c' | sort -n  )


test:
	gcc -o test header.h test.c $(CFLAGS)

all: 
		


clean:
	rm -f test
	rm -rf *.rooms.*
