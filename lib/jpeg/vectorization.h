//
// Created by felipe on 4/9/25.

#ifndef VECTORIZATION_H
#define VECTORIZATION_H
#include <stdint.h>
#include "dct.h"
// WARNING, Cb and Cr components are subject to 4:2:0 undersampling, therefore have half of the height and width
typedef struct {
    int32_t height;
    int32_t width;
    // Each is a matrix of arrays, each array correspond to a single block, vectorized
    int32_t (**Y_block_arrays)[64];
    int32_t (**Cb_block_arrays)[64];
    int32_t (**Cr_block_arrays)[64];
} vectorized_img;

// (De)Allocation functions
vectorized_img* vectorized_img_alloc(int32_t height, int32_t width);
void vectorized_img_free(vectorized_img** img);

// Vecortizes the post quantization DCT image
vectorized_img* vectorize_quantized_img(quantized_dct_image* img);

// Returns the vectorized image to normal shape
quantized_dct_image* devectorize_img(vectorized_img* vec_img);


#endif //VECTORIZATION_H
