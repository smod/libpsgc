CC=arm-linux-gnueabi-gcc
CFLAGS=-Wall -Wextra -pedantic
LDFLAGS=-L. -lpsgc
AR=arm-linux-gnueabi-ar
ARFLAGS=-r

libpsgc.a: psgc.o
	$(AR) $(ARFLAGS) libpsgc.a psgc.o

test: test.o libpsgc.a
	$(CC) test.o -o test $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f libpsgc.a
	rm -f test
	rm -f *.o

.PHONY: clean
