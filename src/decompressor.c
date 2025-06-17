//
// Created by felipe on 6/17/25.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app/app.h"



char helpMessage[] = "Usage: ./<path_to_executable> <src> <dest>\n<src>: path to binary compressed file\n<dest>: path to decompressed .bmp file";

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Error in usage\n");
        printf("%s\n", helpMessage);
        return -1;
    }
    char *src = argv[1];
    char *dest = argv[2];
    bmp_image* bmp = decompress(src);
    bmp_save(bmp, dest);
    bmp_free(&bmp);
}