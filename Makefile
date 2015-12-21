COMPILER = gcc
CFLAGS   = -pthread -Wall -Wunused -Wshadow -pedantic -O2 -std=c99 -g -D_XOPEN_SOURCE
OBJECTS  = muzeum firma bank
OFILES   = queue.o mutex.o helpers1.o helpers2.o err.o
all: $(OBJECTS)

err.o: err.c
	$(COMPILER) $(CFLAGS) -c $^

mutex.o: mutex.c
	$(COMPILER) $(CFLAGS) -c $^

queue.o: queue.c
	$(COMPILER) $(CFLAGS) -c $^

helpers1.o: helpers1.c
	$(COMPILER) $(CFLAGS) -c $^

helpers2.o: helpers2.c
	$(COMPILER) $(CFLAGS) -c $^

bank: $(OFILES) bank.c
	$(COMPILER) $(CFLAGS) -o $@ $^

muzeum: $(OFILES) museum.c
	$(COMPILER) $(CFLAGS) -o $@ $^

firma: $(OFILES) firm.c
	$(COMPILER) $(CFLAGS) -o $@ $^

example%: all example%.sh
	./$@.sh

clean:
	rm -f *.o *.gch $(OBJECTS)

test: clean all
	./example1.sh

kill:
	killall -q -s INT muzeum bank firma

valg:
	valgrind --trace-children=yes ./example1.sh
	sleep 3
	make killvalg

killvalg:
	killall memcheck-amd64-