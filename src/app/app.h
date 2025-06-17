//
// Created by felipe on 6/17/25.
//

#ifndef APP_H
#define APP_H
#include "../imIO/bmp.h"
#include "../imIO//y_cb_cr.h"
#include "../jpeg/dct.h"
#include "../jpeg/diff_rle.h"
#include "../jpeg/vectorization.h"
#include "../jpeg/huffman.h"


// Compress the bmp image at srcFilename and stores result in dstFilename
// On success returns the compression rate, at fails returns < 0
double compress(char* srcFilename, char* dstFilename, double compressionFactor);

// Decompresses the image compressed through this code in srcFilename
// On success returns the bmp_image struct, defined at imIO/bmp.h, on fail returns NULL
bmp_image* decompress(char* srcFilename);


#endif //APP_H
