#include <stdio.h>
#include "lib/imIO/bmp.h"
#include <stdlib.h>
#include "lib/imIO/y_cb_cr.h"
#include "lib/jpeg/dct.h"
#include "lib/jpeg/vectorization.h"

int main(int argc, char *argv[]) {
    bmp_image* bmp = bmp_read("assets/sample2.bmp");
    if (bmp == NULL) {
        perror("Error reading file\n");
        return 1;
    }

    set_compression_factor(strtod(argv[1], NULL));
    YCrCB_image* ycc = bmp_to_yCbCr(bmp);
    dct_YCrCB_image* dcted= DCT_8x8(ycc);
    // int32_t y = 800, x = 640;
    // print_dct_block(dcted, y, x, true, false, false);
    quantized_dct_image* quantized= dct_quantization_8x8(dcted);
    // print_quantized_block(quantized, y, x, true, false, false);
    vectorized_img* vec = vectorize_quantized_img(quantized);
    quantized_dct_image* qt_after = devectorize_img(vec);

    dct_YCrCB_image* dequantized = dequantization_8x8(qt_after);
    // print_dct_block(dequantized, y, x, true, false, false);

    YCrCB_image* idcted = IDCT_8x8(dequantized);


    bmp_image* bmp2 = yCbCr_to_bmp(idcted);
    // printf("%u %u", bmp->infoHeader->horizontalResolution, bmp->infoHeader->verticalResolution);
    bmp_save(bmp2, "rebuilt.bmp");
}