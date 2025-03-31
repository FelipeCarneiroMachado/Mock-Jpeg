bmp:
	gcc -c lib/imIO/bmp.c -o bin/bmp.o

build: bmp
	gcc -c -std=c99 -Wall -Werror  main.c -o bin/main.o
	gcc bin/main.o bin/bmp.o -o main

run:
	./main

clean:
	rm bin/*
	rm main