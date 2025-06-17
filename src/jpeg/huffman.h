#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <stdint.h>
#include "diff_rle.h"
#include <stdlib.h>


/*typedef struct {
    // Dimensions refer to the block matrix, not to the original image dimensions
    // WARNING, Cb and Cr components are subject to 4:2:0 undersampling, therefore have half of the height and width
    int32_t height;
    int32_t width;
    // Each is a matrix of arrays, each array correspond to a single block AC components huffman encoded
    char (**Y_block_AC)[63][27];
    char (**Cb_block_AC)[63][27];
    char (**Cr_block_AC)[63][27];
    // Each is a matrix of ints, each int is the difference encoded DC component of a block 
    char (**Y_block_DC)[19];
    char (**Cb_block_DC)[19];
    char (**Cr_block_DC)[19];
} huffman_img;

//(De)Allocation functions
rlediff_img *huffman_img_alloc(int32_t height, int32_t width);
void huffman_img_free(rlediff_img **huffman_img);*/


//difference encodes the DC components and runlenght encodes the AC components of the vectorized blocks of the image
void huffman_encode(rlediff_img *rlediff_img, char *fname);

//decodes the difference and runlenght encoding
rlediff_img *huffman_decode(char *huffman_name);




#endif