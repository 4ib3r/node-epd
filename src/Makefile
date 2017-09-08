all: test

CC     = gcc
CFLAGS = -I/usr/local/include/freetype2 -I/usr/include/freetype2 -I/usr/include -Werror
LIBS   = -lfreetype

test: test.cc
	$(CC) $(CFLAGS) $< $(LIBS) -o $@
	strip $@

clean:
	rm -f test