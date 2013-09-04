SHELL   = /bin/sh
CFLAGS  = -Wall -g -DNDEBUG -std=c99
CC 	    = cc
DEPS = entry.h chromosome.h genome.h functions.h seatest.h
OBJ = sam2cov.o entry.o chromosome.o genome.o functions.o

TEST_SRC=$(wildcard tests/*.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TARGET=build/libsam2cov.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))
PROG= sam2cov


# The Target Build
all: $(TARGET) $(SO_TARGET) tests binarie

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

binarie:
	$(CC) $(CFLAGS) -o bin/sam2cov $(OBJECTS) $(TARGET)

# The Install
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
	rm -rf bin

#clean:
#	rm -f *.o
#	rm -f sam2cov

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(TARGET)
tests: $(TESTS)
	sh ./tests/runtests.sh

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true