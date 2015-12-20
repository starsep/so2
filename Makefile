COMPILER = gcc
CFLAGS   = -pthread -Wall -Wunused -Wshadow -pedantic -O2 -std=c99 -g
OBJECTS  = muzeum firma bank

all: $(OBJECTS)

err.o: err.c
	$(COMPILER) $(CFLAGS) -c $^

helpers.o: helpers.c
	$(COMPILER) $(CFLAGS) -c $^

bank: helpers.o err.o bank.c
	$(COMPILER) $(CFLAGS) -o $@ $^

muzeum: helpers.o err.o museum.c
	$(COMPILER) $(CFLAGS) -o $@ $^

firma: helpers.o err.o firma.c
	$(COMPILER) $(CFLAGS) -o $@ $^

example%: all example%.sh
	./$@.sh

clean:
	rm -f *.o *.gch $(OBJECTS)
