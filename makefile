SHELL   = /bin/sh
CFLAGS  = -Wall -g -DNDEBUG -std=c99
CC 	    = cc
DEPS = entry.h chromosome.h genome.h functions.h
OBJ = sam2cov.o entry.o chromosome.o genome.o functions.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sam2cov: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o
	rm -f sam2cov
