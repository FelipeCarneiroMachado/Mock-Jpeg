# Project Makefile

CFLAGS = -Wall -O2 -g -Wextra
LDFLAGS = -lm


dct:
	gcc -c $(CFLAGS) lib/jpeg/dct.c -o bin/dct.o

bmp:
	gcc -c $(CFLAGS) lib/imIO/bmp.c -o bin/bmp.o

y_cb_cr:
	gcc -c $(CFLAGS) lib/imIO/y_cb_cr.c -o bin/y_cb_cr.o

build: bmp y_cb_cr dct
	gcc -c $(CFLAGS)   main.c -o bin/main.o
	gcc  bin/main.o bin/bmp.o bin/y_cb_cr.o bin/dct.o $(LDFLAGS) -o main

run:
	./main

clean:
	rm bin/*
	rm main