#ifndef HUFFMAN_H
#define HUFFMAN_H
#include <stdint.h>
#include "diff_rle.h"
#include <stdlib.h>




//huffman encodes the AC and DC components of a struct that holds them runlength and difference encoded into a binary file
void huffman_encode(rlediff_img *rlediff_img, char *fname);

//decodes the huffman into a struct that holds the difference and runlenght encoded DC and AC components
rlediff_img *huffman_decode(char *huffman_name);


#endif