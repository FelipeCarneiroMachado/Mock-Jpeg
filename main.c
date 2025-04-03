#include <stdio.h>
#include "lib/imIO/bmp.h"
#include <stdlib.h>
#include "lib/imIO/y_cb_cr.h"
#include "lib/jpeg/dct.h"


int main(void) {
    bmp_image* bmp = bmp_read("assets/sample2.bmp");
    if (bmp == NULL) {
        perror("Error reading file\n");
        return 1;
    }

    set_compression_factor(1.0);
    YCrCB_image* ycc = bmp_to_yCbCr(bmp);
    dct_YCrCB_image* dcted= DCT_8x8(ycc);
    quantized_dct_image* quantized= dct_quantization_8x8(dcted);
    dct_YCrCB_image* dequantized = dequantization_8x8(quantized);
    YCrCB_image* idcted = IDCT_8x8(dequantized);


    bmp_image* bmp2 = yCbCr_to_bmp(idcted);
    // printf("%u %u", bmp->infoHeader->horizontalResolution, bmp->infoHeader->verticalResolution);
    bmp_save(bmp2, "rebuilt.bmp");
}