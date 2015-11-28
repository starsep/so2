COMPILER = gcc
CFLAGS   = -pthread -Wall -Wunused -Wshadow -pedantic -O2 -std=c99 -g
OBJECTS  = muzeum firma bank

all: $(OBJECTS)

bank: bank.c
	$(COMPILER) $(CFLAGS) -o $@ $^

muzeum: muzeum.c
	$(COMPILER) $(CFLAGS) -o $@ $^

firma: firma.c
	$(COMPILER) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(OBJECTS)
