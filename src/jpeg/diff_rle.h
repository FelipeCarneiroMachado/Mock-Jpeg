
#ifndef DIFF_RLE_H
#define DIFF_RLE_H
#include <stdint.h>
#include "vectorization.h"

typedef struct {
    int32_t first;
    int32_t second;
} pair;

typedef struct {
    // Dimensions refer to the block matrix, not to the original image dimensions
    // WARNING, Cb and Cr components are subject to 4:2:0 undersampling, therefore have half of the height and width
    int32_t height;
    int32_t width;
    float compression_factor;
    // Each is a matrix of arrays, each array correspond to a single block AC components vectorized and run lenght enconded
    pair (**Y_block_AC)[63];
    pair (**Cb_block_AC)[63];
    pair (**Cr_block_AC)[63];
    // Each is a matrix of ints, each int is the difference encoded DC component of a block 
    int32_t **Y_block_DC;
    int32_t **Cb_block_DC;
    int32_t **Cr_block_DC;
} rlediff_img;

// (De)Allocation functions
rlediff_img *rlediff_img_alloc(int32_t height, int32_t width, float compression_factor);
void rlediff_img_free(rlediff_img **rlediff_img);


//difference encodes the DC components and runlenght encodes the AC components of the vectorized blocks of the image
rlediff_img *partial_encode(vectorized_img *vectorized_img);

//decodes the difference and runlenght encoding
vectorized_img *partial_decode(rlediff_img *rlediff_img);




#endif