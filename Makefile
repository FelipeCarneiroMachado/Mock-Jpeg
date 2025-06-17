# Project Makefile

CFLAGS = -Wall -O2 -g -Wextra
LDFLAGS = -lm


dct:
	gcc -c $(CFLAGS) src/jpeg/dct.c -o bin/dct.o

bmp:
	gcc -c $(CFLAGS) src/imIO/bmp.c -o bin/bmp.o

y_cb_cr:
	gcc -c $(CFLAGS) src/imIO/y_cb_cr.c -o bin/y_cb_cr.o

vectorization:
	gcc -c $(CFLAGS) src/jpeg/vectorization.c -o bin/vectorization.o

diff_rle:
	gcc -c $(CFLAGS) src/jpeg/diff_rle.c -o bin/diff_rle.o

huffman:
	gcc -c $(CFLAGS) src/jpeg/huffman.c -o bin/huffman.o


build: bmp y_cb_cr dct vectorization diff_rle huffman
	gcc -c $(CFLAGS)   src/main.c -o bin/main.o
	gcc  bin/main.o bin/bmp.o bin/y_cb_cr.o bin/dct.o bin/vectorization.o bin/diff_rle.o bin/huffman.o $(LDFLAGS) -o bin/main

run:
	./bin/main 2.0 >results2.txt

clean:
	rm bin/*
	rm main

all:
	make clean
	make build
	make run