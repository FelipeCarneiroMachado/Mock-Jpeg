//
// Created by felipe on 6/17/25.
//

#include "app.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>


// Utility function to get file size in bytes
long get_file_size(const char *filename) {
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    // Error occurred
    fprintf(stderr, "Error getting file size for '%s': %s\n",
            filename, strerror(errno));
    return -1;
}



double compress(char* srcFilename, char* dstFilename, double compressionFactor) {
    // Image acquisition
    bmp_image* src_bmp = bmp_read(srcFilename);
    if (src_bmp == NULL) {
        printf("Error reading file");
    }
    // Compression factor set globally
    set_compression_factor(compressionFactor);
    // Color space conversion (RGB -> YCbCr) with downsampling
    YCrCB_image* post_yuv = bmp_to_yCbCr(src_bmp);
    // DCT application
    dct_YCrCB_image* post_dct = DCT_8x8(post_yuv);
    // Aplly quantization
    quantized_dct_image* quantized = dct_quantization_8x8(post_dct);
    // Apply zig-zag vectorization
    vectorized_img* vectorized = vectorize_quantized_img(quantized);
    // Run length encoding and differential encoding
    rlediff_img* rlediff = partial_encode(vectorized);
    // Huffman encoding and file writing
    huffman_encode(rlediff,dstFilename);

    // Returns the ratio of the file sizes
    return double(get_file_size(dstFilename)) / double(get_file_size(srcFilename));
}


bmp_image* decompress(char* srcFilename) {
    // Reads from file and converts to run length + differential encoding
    rlediff_img* rlediff = huffman_decode(srcFilename);
    // TODO
    set_compression_factor(1.0);
    vectorized_img* vectorized = partial_decode(rlediff);
    quantized_dct_image*  quantized = devectorize_img(vectorized);
    // TODO
}