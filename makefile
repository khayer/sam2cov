SHELL   = /bin/sh
CFLAGS  = -Wall -g -DNDEBUG
HEADERS = entry.h
OBJECTS = entry.o
CC 	    = cc
DEPS = entry.h chromosome.h genome.h
OBJ = sam2cov.o entry.o chromosome.o genome.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sam2cov: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f sam2cov
