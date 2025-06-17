//
// Created by felipe on 6/17/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app/app.h"



char helpMessage[] = "Usage: ./<path_to_executable> <src> <dest> <compression_factor>\n<src>: source .bmp file path\n<dest>: file path for compressed image\n<compression_factor>: compression factor for quantization";

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Error in usage\n");
        printf("%s\n", helpMessage);
        return -1;
    }
    char *src = argv[1];
    char *dest = argv[2];
    double compression_factor = atof(argv[3]);
    double compression_rate = compress(src, dest, compression_factor);
    printf("Compression sucessuful\nRate achieved: %.2f%%\n", 100 - (compression_rate * 100));
}