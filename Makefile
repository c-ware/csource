OBJS=src/main.o src/cstring/cstring.o src/libmatch/read.o src/libmatch/cond.o src/libmatch/cursor.o src/libmatch/match.o src/libpath/iter.o src/libpath/libpath.o src/libpath/compile/error.o src/libpath/compile/path.o src/libpath/compile/backends/unix.o src/argparse/ap_inter.o src/argparse/extract.o src/argparse/argparse.o src/extractors/include/include.o 
TESTOBJS=src/cstring/cstring.o src/libmatch/read.o src/libmatch/cond.o src/libmatch/cursor.o src/libmatch/match.o src/libpath/iter.o src/libpath/libpath.o src/libpath/compile/error.o src/libpath/compile/path.o src/libpath/compile/backends/unix.o src/argparse/ap_inter.o src/argparse/extract.o src/argparse/argparse.o src/extractors/include/include.o 
TESTS=
CC=cc
PREFIX=/usr/local
LDFLAGS=
LDLIBS=
CFLAGS=

all: $(OBJS) $(TESTS) csource

clean:
	rm -rf $(OBJS)
	rm -rf $(TESTS)
	rm -rf vgcore.*
	rm -rf core*
	rm -rf csource

install:
	mkdir -p $(PREFIX)
	install -m 755 csource $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/csource

src/main.o: src/main.c src/csource.h src/extractors/include/include.h
	$(CC) -c $(CFLAGS) src/main.c -o src/main.o

src/cstring/cstring.o: src/cstring/cstring.c src/cstring/cstring.h
	$(CC) -c $(CFLAGS) src/cstring/cstring.c -o src/cstring/cstring.o

src/libmatch/read.o: src/libmatch/read.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/read.c -o src/libmatch/read.o

src/libmatch/cond.o: src/libmatch/cond.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/cond.c -o src/libmatch/cond.o

src/libmatch/cursor.o: src/libmatch/cursor.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/cursor.c -o src/libmatch/cursor.o

src/libmatch/match.o: src/libmatch/match.c src/libmatch/libmatch.h
	$(CC) -c $(CFLAGS) src/libmatch/match.c -o src/libmatch/match.o

src/libpath/iter.o: src/libpath/iter.c
	$(CC) -c $(CFLAGS) src/libpath/iter.c -o src/libpath/iter.o

src/libpath/libpath.o: src/libpath/libpath.c src/libpath/libpath.h src/libpath/lp_inter.h
	$(CC) -c $(CFLAGS) src/libpath/libpath.c -o src/libpath/libpath.o

src/libpath/compile/error.o: src/libpath/compile/error.c src/libpath/compile/path.h src/libpath/libpath.h src/libpath/lp_inter.h
	$(CC) -c $(CFLAGS) src/libpath/compile/error.c -o src/libpath/compile/error.o

src/libpath/compile/path.o: src/libpath/compile/path.c src/libpath/compile/path.h src/libpath/libpath.h src/libpath/lp_inter.h
	$(CC) -c $(CFLAGS) src/libpath/compile/path.c -o src/libpath/compile/path.o

src/libpath/compile/backends/unix.o: src/libpath/compile/backends/unix.c src/libpath/compile/path.h src/libpath/libpath.h src/libpath/lp_inter.h
	$(CC) -c $(CFLAGS) src/libpath/compile/backends/unix.c -o src/libpath/compile/backends/unix.o

src/argparse/ap_inter.o: src/argparse/ap_inter.c src/argparse/argparse.h src/argparse/ap_inter.h
	$(CC) -c $(CFLAGS) src/argparse/ap_inter.c -o src/argparse/ap_inter.o

src/argparse/extract.o: src/argparse/extract.c src/argparse/argparse.h src/argparse/ap_inter.h
	$(CC) -c $(CFLAGS) src/argparse/extract.c -o src/argparse/extract.o

src/argparse/argparse.o: src/argparse/argparse.c src/argparse/argparse.h src/argparse/ap_inter.h
	$(CC) -c $(CFLAGS) src/argparse/argparse.c -o src/argparse/argparse.o

src/extractors/include/include.o: src/extractors/include/include.c src/extractors/include/include.h src/csource.h
	$(CC) -c $(CFLAGS) src/extractors/include/include.c -o src/extractors/include/include.o

csource: $(OBJS)
	$(CC) $(OBJS) -o csource $(LDFLAGS) $(LDLIBS)
