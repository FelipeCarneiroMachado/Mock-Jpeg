# Project Makefile

CFLAGS = -Wall -O0 -g -Wextra
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

app:
	gcc -c $(CFLAGS) src/app/app.c -o bin/app.o

dirs:
	mkdir -p bin executables

build: dirs bmp y_cb_cr dct vectorization diff_rle huffman app
	gcc -c $(CFLAGS)   src/compressor.c -o bin/compressor.o
	gcc -c $(CFLAGS)   src/decompressor.c -o bin/decompressor.o
	gcc  bin/compressor.o bin/app.o bin/bmp.o bin/y_cb_cr.o bin/dct.o bin/vectorization.o bin/diff_rle.o bin/huffman.o $(LDFLAGS) -o executables/compressor
	gcc  bin/decompressor.o bin/app.o bin/bmp.o bin/y_cb_cr.o bin/dct.o bin/vectorization.o bin/diff_rle.o bin/huffman.o $(LDFLAGS) -o executables/decompressor


clean:
	rm bin/*
	rm executables/*

