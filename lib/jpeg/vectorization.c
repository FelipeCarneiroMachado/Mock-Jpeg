//
// Created by felipe on 4/9/25.
//


#include "vectorization.h"

#include <stdlib.h>

// Cordinate struct
typedef struct {
    int32_t y;
    int32_t x;
} pair;

// Hardcoded vectorization pattern
pair zigzag_pattern[64] = {
    {0, 0},
    {0, 1},
    {1, 0},
    {2, 0},
    {1, 1},
    {0, 2},
    {0, 3},
    {1, 2},
    {2, 1},
    {3, 0},
    {4, 0},
    {3, 1},
    {2, 2},
    {1, 3},
    {0, 4},
    {0, 5},
    {1, 4},
    {2, 3},
    {3, 2},
    {4, 1},
    {5, 0},
    {6, 0},
    {5, 1},
    {4, 2},
    {3, 3},
    {2, 4},
    {1, 5},
    {0, 6},
    {0, 7},
    {1, 6},
    {2, 5},
    {3, 4},
    {4, 3},
    {5, 2},
    {6, 1},
    {7, 0},
    {7, 1},
    {6, 2},
    {5, 3},
    {4, 4},
    {3, 5},
    {2, 6},
    {1, 7},
    {2, 7},
    {3, 6},
    {4, 5},
    {5, 4},
    {6, 3},
    {7, 2},
    {7, 3},
    {6, 4},
    {5, 5},
    {4, 6},
    {3, 7},
    {4, 7},
    {5, 6},
    {6, 5},
    {7, 4},
    {7, 5},
    {6, 6},
    {5, 7},
    {6, 7},
    {7, 6},
    {7, 7}
};


int32_t (**alloc_block_matrix(int32_t height, int32_t width))[64] {
    int32_t (**mat)[64];
    mat = malloc(height * sizeof(int32_t (*)[64]));
    for (int32_t i = 0; i < height; i++) {
        mat[i] = malloc(width * sizeof(int32_t [64]));
    }
    return mat;
}

vectorized_img* vectorized_img_alloc(int32_t height, int32_t width) {
    vectorized_img* vec = (vectorized_img*)malloc(sizeof(vectorized_img));
    vec->height = height;
    vec->width = width;

    vec->Y_block_arrays = alloc_block_matrix(height, width);
    vec->Cb_block_arrays = alloc_block_matrix(height / 2, width / 2);
    vec->Cr_block_arrays = alloc_block_matrix(height / 2, width / 2);

    return vec;
}

void block_matrix_free(int32_t (**mat)[64], int32_t height, int32_t width) {
    for (int32_t i = 0; i < height; i++) {
        free(mat[i]);
    }
    free(mat);
}

void vectorized_img_free(vectorized_img** img) {
    block_matrix_free((*img)->Y_block_arrays, (*img)->height, (*img)->width);
    block_matrix_free((*img)->Cb_block_arrays, (*img)->height / 2, (*img)->width / 2);
    block_matrix_free((*img)->Cr_block_arrays, (*img)->height / 2, (*img)->width / 2);
    free(*img);
    *img = NULL;
}


void vectorize_block_at(quantized_px_t** src, int32_t dest[64], int32_t y, int32_t x) {
    for (int32_t i = 0; i < 64; i++) {
        dest[i] = src[zigzag_pattern[i].y][zigzag_pattern[i].x];
    }
}

vectorized_img* vectorize_quantized_img(quantized_dct_image* img) {
    vectorized_img* vec = vectorized_img_alloc(img->height, img->width);
    for (int32_t i = 0; i < img->height; i+=8) {
        for (int32_t j = 0; j < img->width; j+=8) {
            vectorize_block_at(img->Y_quantized, vec->Y_block_arrays[i][j], i, j);
        }
    }
    for (int32_t i = 0; i < img->height / 2; i+=8) {
        for (int32_t j = 0; j < img->width / 2; j+=8) {
            vectorize_block_at(img->Cb_quantized, vec->Cb_block_arrays[i][j], i, j);
            vectorize_block_at(img->Cr_quantized, vec->Cr_block_arrays[i][j], i, j);
        }
    }
    return vec;
}





