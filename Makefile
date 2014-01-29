#

VERSION=1.0

#CC=gcc -g -O0
CC=gcc -O3
LD=gcc

CFLAGS=-ggdb -Wall -Wextra -Waggregate-return -Wformat -Wshadow -Wconversion \
	-Wredundant-decls -Wpointer-arith -Wcast-align -pedantic -O2

INCLUDES=`gimptool-2.0 --cflags`
LDFLAGS=`gimptool-2.0 --libs`

GLIB_INCLUDES=`pkg-config --cflags gtk+-2.0 gobject-2.0`
GLIB_LDFLAGS=`pkg-config --libs gtk+-2.0 gobject-2.0`

OFILES= main.o \
	expr.o \
	expr-math.o \
	toagtk.o \
	gundo.o \
	toaeditor.o \
	toastring.o

sinxpi: $(OFILES)
	$(LD) -o sinxpi $(OFILES) $(LDFLAGS) -lm

expr-test: expr.c expr.h expr-math.o expr-optab.inc
	$(CC) -DTEST $(CFLAGS) -o expr-test expr.c expr-math.o -lm

math-test: expr-math.c expr-math.h
	$(CC) -DTEST $(CFLAGS) -o math-test expr-math.c -lm

str-test: toastring.c toastring.h
	$(CC) -DTEST $(CFLAGS) -o str-test toastring.c

gundo-test: gundo.c gundo.h
	$(CC) -DG_UNDO_LIST_TEST $(GLIB_INCLUDES) $(CFLAGS) -o gundo-test gundo.c $(GLIB_LDFLAGS)

expr.o: expr.c expr.h expr-optab.inc
	$(CC) $(CFLAGS) -o expr.o -c expr.c

expr-math.o: expr-math.c expr-math.h
	$(CC) $(CFLAGS) -o expr-math.o -c expr-math.c

gundo.o: gundo.c gundo.h
	$(CC) $(INCLUDES) $(CFLAGS) -o gundo.o -c gundo.c

main.o: main.c expr.h toastring.h expr-defs.inc expr-optab.inc
	$(CC) $(INCLUDES) $(CFLAGS) -o main.o -c main.c

toaeditor.o: toaeditor.c toaeditor.h
	$(CC) $(INCLUDES) $(CFLAGS) -o toaeditor.o -c toaeditor.c

toagtk.o: toagtk.c toagtk.h
	$(CC) $(INCLUDES) $(CFLAGS) -o toagtk.o -c toagtk.c

toastring.o: toastring.c toastring.h
	$(CC) $(INCLUDES) $(CFLAGS) -o toastring.o -c toastring.c

install: sinxpi
	gimptool-2.0 --install-bin sinxpi

uninstall: sinxpi
	gimptool-2.0 --uninstall-bin sinxpi

clean:
	rm -f *.o sinxpi expr-test math-test str-test

dist:
	mkdir -p sinxpi-$(VERSION)
	cp *.c *.h *.inc COPYING README INSTALL Makefile sinxpi-$(VERSION)/
	tar -czvf sinxpi-$(VERSION).tar.gz sinxpi-$(VERSION)/
	rm -rf sinxpi-$(VERSION)/
