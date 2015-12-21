COMPILER = gcc
CFLAGS   = -pthread -Wall -Wunused -Wshadow -pedantic -O2 -std=c99 -g -D_XOPEN_SOURCE
OBJECTS  = bank muzeum firma
OFILES   = queue.o helpers1.o helpers2.o err.o
all: $(OBJECTS)

err.o: err.c
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

clean:
	rm -f *.o *.gch $(OBJECTS)
