SHELL   = /bin/sh
CFLAGS  = -Wall -g -DNDEBUG -std=c99
CC 	    = cc
DEPS = entry.h chromosome.h genome.h functions.h seatest.h
OBJ = sam2cov.o entry.o chromosome.o genome.o functions.o seatest.o test.o


%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sam2cov: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test:
	$(CC) -o $@ test.o seatest.o $(CFLAGS)

clean:
	rm -f *.o
	rm -f sam2cov
	rm -f test
