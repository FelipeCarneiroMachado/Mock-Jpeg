//
// Created by felipe on 3/31/25.
//



/*
 * Manipulation of YCrCb images, does not read/write from/to files of any format
 * Converts from/to bmp images defined in bmp.h, meant to be used in the DCT process
 * WARNING!!!!: Cr and Cb components are undersampled in 4:2:0 proportion
 * You are strongly advised to use the get_Cr and get_Cb functions to access them
 */

#ifndef Y_CB_CR_H
#define Y_CB_CR_H
#include <stdint.h>
#include "bmp.h"

    //Pixel type definition for semantics
    typedef double yCbCr_px;


    // Image struct
    typedef struct {
        int32_t width;
        int32_t height;
        yCbCr_px** Y_data;
        yCbCr_px** Cb_data;
        yCbCr_px** Cr_data;
    }YCrCB_image;

    // Undersampled component access

    yCbCr_px get_Cr(YCrCB_image* img, int32_t x, int32_t y);

    yCbCr_px get_Cb(YCrCB_image* img, int32_t x, int32_t y);

    // Deallocation
    void free_yCbCr_matrix(yCbCr_px*** matrix, int32_t width, int32_t height);

    // Allocation and initialization
    YCrCB_image* newYCrCB_image(int32_t width, int32_t height);

    // bmp -> YCrCb conversion
    YCrCB_image *bmp_to_yCbCr(bmp_image* img);

    // YCrCb -> bmp conversion
    bmp_image* yCbCr_to_bmp(YCrCB_image* img);

#endif //Y_CB_CR_H
