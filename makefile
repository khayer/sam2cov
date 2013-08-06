SHELL = /bin/sh
CFLAGS=-Wall -g -DNDEBUG

all: sam2cov

clean:
	rm -f sam2cov
	rm -r *.dSYM
