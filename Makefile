# Project Makefile

CFLAGS = -Wall -O2 -g -Wextra
LDFLAGS = -lm


dct:
	gcc -c $(CFLAGS) lib/jpeg/dct.c -o bin/dct.o

bmp:
	gcc -c $(CFLAGS) lib/imIO/bmp.c -o bin/bmp.o

y_cb_cr:
	gcc -c $(CFLAGS) lib/imIO/y_cb_cr.c -o bin/y_cb_cr.o

vectorization:
	gcc -c $(CFLAGS) lib/jpeg/vectorization.c -o bin/vectorization.o

diff_rle:
	gcc -c $(CFLAGS) lib/jpeg/diff_rle.c -o bin/diff_rle.o


build: bmp y_cb_cr dct vectorization diff_rle
	gcc -c $(CFLAGS)   main.c -o bin/main.o
	gcc  bin/main.o bin/bmp.o bin/y_cb_cr.o bin/dct.o bin/vectorization.o bin/diff_rle.o $(LDFLAGS) -o main

run:
	./main 8.0

clean:
	rm bin/*
	rm main

all:
	make clean
	make build
	make run