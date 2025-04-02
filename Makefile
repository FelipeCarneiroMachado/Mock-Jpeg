# Project Makefile

CFLAGS = -Wall -O2 -lm -Wextra -g

bmp:
	gcc -c $(CFLAGS) lib/imIO/bmp.c -o bin/bmp.o

y_cb_cr:
	gcc -c $(CFLAGS) lib/imIO/y_cb_cr.c -o bin/y_cb_cr.o

build: bmp y_cb_cr
	gcc -c $(CFLAGS) -std=c99 -Wall  main.c -o bin/main.o
	gcc bin/main.o bin/bmp.o bin/y_cb_cr.o -o main

run:
	./main

clean:
	rm bin/*
	rm main