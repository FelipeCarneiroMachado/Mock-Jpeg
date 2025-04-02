#include <stdio.h>
#include "lib/imIO/bmp.h"
#include <stdlib.h>
#include "lib/imIO/y_cb_cr.h"

int main(void) {
    bmp_image* bmp = bmp_read("assets/3colors.bmp");
    if (bmp == NULL) {
        perror("Error reading file\n");
        return 1;
    }
    YCrCB_image* ycc = bmp_to_yCbCr(bmp);
    bmp_image* bmp2 = yCbCr_to_bmp(ycc);
    // printf("%u %u", bmp->infoHeader->horizontalResolution, bmp->infoHeader->verticalResolution);
    bmp_save(bmp2, "rebuilt.bmp");
}