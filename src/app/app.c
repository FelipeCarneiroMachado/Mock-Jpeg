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


    // Struct deallocation
    bmp_free(&src_bmp);

    rlediff_img_free(&rlediff);

    vectorized_img_free(&vectorized);
    free_quantized_px_t_matrix(&quantized->Y_quantized, quantized->height);
    free_quantized_px_t_matrix(&quantized->Cr_quantized, quantized->height / 2);
    free_quantized_px_t_matrix(&quantized->Cb_quantized, quantized->height / 2);
    free(quantized);
    free_yCbCr_matrix(&post_dct->Y_data, post_dct->width, post_dct->height);
    free_yCbCr_matrix(&post_dct->Cb_data, post_dct->width / 2, post_dct->height / 2);
    free_yCbCr_matrix(&post_dct->Cr_data, post_dct->width / 2, post_dct->height / 2);
    free(post_dct);
    free_yCbCr_matrix(&post_yuv->Y_data, post_yuv->width, post_yuv->height);
    free_yCbCr_matrix(&post_yuv->Cb_data, post_yuv->width / 2, post_yuv->height / 2);
    free_yCbCr_matrix(&post_yuv->Cr_data, post_yuv->width / 2, post_yuv->height / 2);
    free(post_yuv);


    // Returns the ratio of the file sizes
    return (double)(get_file_size(dstFilename)) / (double)(get_file_size(srcFilename));
}


bmp_image* decompress(char* srcFilename) {
    // Reads from file and converts to run length + differential encoding
    rlediff_img* rlediff = huffman_decode(srcFilename);
    // Sets compression factor for dequantization
    set_compression_factor(rlediff->compression_factor);
    // Reverse run length / differential encoding
    vectorized_img* vectorized = partial_decode(rlediff);
    // Reverse zig-zag vectorization
    quantized_dct_image*  quantized = devectorize_img(vectorized);
    // Reverse quantization
    dct_YCrCB_image* dequantized = dequantization_8x8(quantized);
    // Inverse DCT
    YCrCB_image* yuv = IDCT_8x8(dequantized);
    // Color Space conversion (YCbCr -> RGB)
    bmp_image* bmp = yCbCr_to_bmp(yuv);


    // Struct deallocation
    rlediff_img_free(&rlediff);
    vectorized_img_free(&vectorized);
    free_quantized_px_t_matrix(&quantized->Y_quantized, quantized->height);
    free_quantized_px_t_matrix(&quantized->Cr_quantized, quantized->height / 2);
    free_quantized_px_t_matrix(&quantized->Cb_quantized, quantized->height / 2);
    free(quantized);
    free_yCbCr_matrix(&dequantized->Y_data, dequantized->width, dequantized->height);
    free_yCbCr_matrix(&dequantized->Cb_data, dequantized->width / 2, dequantized->height / 2);
    free_yCbCr_matrix(&dequantized->Cr_data, dequantized->width / 2, dequantized->height / 2);
    free(dequantized);
    free_yCbCr_matrix(&yuv->Y_data, yuv->width / 2, yuv->height / 2);
    free_yCbCr_matrix(&yuv->Cb_data, yuv->width / 2, yuv->height / 2);
    free_yCbCr_matrix(&yuv->Cr_data, yuv->width / 2, yuv->height / 2);
    free(yuv);

    return bmp;

}