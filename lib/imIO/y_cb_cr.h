//
// Created by felipe on 3/31/25.
//

#ifndef Y_CB_CR_H
#define Y_CB_CR_H
#include <stdint.h>

#include "bmp.h"

typedef float yCbCr_px;

    typedef struct {
        int32_t width;
        int32_t height;
        yCbCr_px** Y_data;
        yCbCr_px** Cb_data;
        yCbCr_px** Cr_data;
    }YCrCB_image;


    yCbCr_px get_Cr(YCrCB_image* img, int32_t x, int32_t y);

    yCbCr_px get_Cb(YCrCB_image* img, int32_t x, int32_t y);

    void free_yCbCr_matrix(yCbCr_px*** matrix, int32_t width, int32_t height);

    YCrCB_image* newYCrCB_image(int32_t width, int32_t height)

    YCrCB_image *bmp_to_yCbCr(bmp_image* img);

#endif //Y_CB_CR_H
