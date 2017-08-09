CC=gcc
CFLAGS= -Wall -lm
LDFLAGS=
SOURCES=smallsh.c
EXECUTABLE=xsmallsh

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): 
	    $(CC) $(CFLAGS) $(SOURCES) -o $@

run: all
	./$(EXECUTABLE)

.PHONY: all run

clean: 
	rm -f $(EXECUTABLE) *.o

cleaner: clean all run
