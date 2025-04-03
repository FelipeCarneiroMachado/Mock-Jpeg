//
// Created by felipe on 3/31/25.
//

/*
 * Implements the Discrete Cosine Transform (DCT) for the JPEG compression
 * Uses YCrCb images (defined in y_cr_cb.h) as input
 */

#ifndef DCT_H
#define DCT_H
#include "../imIO/y_cb_cr.h"
#include <stdint.h>


    // Same structure, only used for code semantic
    // For allocation and deallocation memory the same functions defined in y_cr_cb.h
    typedef YCrCB_image dct_YCrCB_image;

    // Semantic type for quantized pixels
    typedef uint8_t quantized_px_t;


    // Return type for the quantization process
    typedef struct {
        int32_t width;
        int32_t height;
        quantized_px_t** Y_quantized;
        quantized_px_t** Cr_quantized;
        quantized_px_t** Cb_quantized;
    } quantized_dct_image;

    // Sets globally the compression factor
    void set_compression_factor(double factor);


    // Applies DCT in 8x8 blocks on an YCrCb image
    dct_YCrCB_image* DCT_8x8(YCrCB_image* img);

    YCrCB_image* IDCT_8x8(dct_YCrCB_image* img);

    // Applies quantization to the result of the 8x8 DCT
    quantized_dct_image* dct_quantization_8x8(dct_YCrCB_image *img);

    dct_YCrCB_image* dequantization_8x8(quantized_dct_image* img);

#endif //DCT_H
