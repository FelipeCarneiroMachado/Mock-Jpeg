#include <stdio.h>
#include "lib/imIO/bmp.h"
#include <stdlib.h>

int main(void) {
    bmp_image* bmp = bmp_read("assets/3colors.bmp");
    if (bmp == NULL) {
        perror("Error reading file\n");
    }
}