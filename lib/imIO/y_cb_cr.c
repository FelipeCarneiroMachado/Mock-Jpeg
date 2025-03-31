//
// Created by felipe on 3/31/25.
//

#include "y_cb_cr.h"

#include <stdlib.h>




yCbCr_px** yCbCr_matrix_alloc(int32_t width, int32_t height) {
    yCbCr_px** matrix = (yCbCr_px**)malloc(sizeof(yCbCr_px*) * width);
    for (int i = 0; i < width; i++) {
        matrix[i] = (yCbCr_px*)malloc(sizeof(yCbCr_px*) * height);
    }
    return matrix;
}

void free_yCbCr_matrix(yCbCr_px*** matrix, int32_t width, int32_t height) {
    for (int i = 0; i < width; i++) {
        free((*matrix)[i]);
    }
    free(*matrix);
    *matrix = NULL;
}

YCrCB_image* newYCrCB_image(int32_t width, int32_t height) {
    YCrCB_image* img = (YCrCB_image*)malloc(sizeof(YCrCB_image));
    img->width = width;
    img->height = height;
    img->Y_data = yCbCr_matrix_alloc(width, height);
    img->Cb_data = yCbCr_matrix_alloc(width / 2, height / 2);
    img->Cr_data = yCbCr_matrix_alloc(width / 2, height / 2);
    return img;
}

void free_yCbCr_image(YCrCB_image** img) {
    free_yCbCr_matrix(&(*img)->Y_data, (*img)->width, (*img)->height);
    free_yCbCr_matrix(&(*img)->Cb_data, (*img)->width / 2, (*img)->height / 2);
    free_yCbCr_matrix(&(*img)->Cr_data, (*img)->width / 2, (*img)->height / 2);
    free(*img);
}

yCbCr_px get_Cr(YCrCB_image* img, int32_t x, int32_t y) {
    return img->Cr_data[x / 2][y / 2];
}

yCbCr_px get_Cb(YCrCB_image* img, int32_t x, int32_t y) {
    return img->Cb_data[x / 2][y / 2];
}



YCrCB_image* bmp_to_yCbCr(bmp_image* img) {
    YCrCB_image* new_img = newYCrCB_image(img->infoHeader->width, img->infoHeader->height);
    for (int i = 0; i < img->infoHeader->width; i++) {
        for (int j = 0; j < img->infoHeader->height; j++) {
            new_img->Y_data[i][j] = (yCbCr_px) 0.299 * img->r_data[i][j] + 0.587 * img->g_data[i][j] + 0.114 * img->b_data[i][j];
            if (i % 2 == 0 && j % 2 == 0) {
                new_img->Cb_data[i / 2][j / 2] = (yCbCr_px) 0.564 * (img->b_data[i][j] - new_img->Y_data[i][j]);
                new_img->Cr_data[i / 2][j / 2] = (yCbCr_px) 0.713 * (img->r_data[i][j] - new_img->Y_data[i][j]);
            }
        }
    }
    return new_img;
}


