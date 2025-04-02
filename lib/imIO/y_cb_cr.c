//
// Created by felipe on 3/31/25.
//

#include "y_cb_cr.h"

#include <stdlib.h>



// Allocation for pixel data
yCbCr_px** yCbCr_matrix_alloc(int32_t width, int32_t height) {
    yCbCr_px** matrix = (yCbCr_px**)malloc(sizeof(yCbCr_px*) * height);
    for (int i = 0; i < height; i++) {
        matrix[i] = (yCbCr_px*)malloc(sizeof(yCbCr_px) * width);
    }
    return matrix;
}

// Deallocation for pixel data
void free_yCbCr_matrix(yCbCr_px*** matrix, int32_t width, int32_t height) {
    for (int i = 0; i < width; i++) {
        free((*matrix)[i]);
    }
    free(*matrix);
    *matrix = NULL;
}

// Initializatio and alllocation for images
YCrCB_image* newYCrCB_image(int32_t width, int32_t height) {
    YCrCB_image* img = (YCrCB_image*)malloc(sizeof(YCrCB_image));
    img->width = width;
    img->height = height;
    img->Y_data = yCbCr_matrix_alloc(width, height);
    img->Cb_data = yCbCr_matrix_alloc(width / 2, height / 2);
    img->Cr_data = yCbCr_matrix_alloc(width / 2, height / 2);
    return img;
}

// Deallocation for images
void free_yCbCr_image(YCrCB_image** img) {
    free_yCbCr_matrix(&(*img)->Y_data, (*img)->width, (*img)->height);
    free_yCbCr_matrix(&(*img)->Cb_data, (*img)->width / 2, (*img)->height / 2);
    free_yCbCr_matrix(&(*img)->Cr_data, (*img)->width / 2, (*img)->height / 2);
    free(*img);
}

// Safe access to undersampled components
yCbCr_px get_Cr(YCrCB_image* img, int32_t y, int32_t x) {
    return img->Cr_data[y / 2][x / 2];
}

yCbCr_px get_Cb(YCrCB_image* img, int32_t y, int32_t x) {
    return img->Cb_data[y / 2][x / 2];
}


// Conversion bmp -> YCrCb
YCrCB_image* bmp_to_yCbCr(bmp_image* img) {
    YCrCB_image* new_img = newYCrCB_image(img->infoHeader->width, img->infoHeader->height);
    for (int i = 0; i < img->infoHeader->height; i++) {
        for (int j = 0; j < img->infoHeader->width; j++) {
            new_img->Y_data[i][j] = (yCbCr_px) 0.299 * img->r_data[i][j] + 0.587 * img->g_data[i][j] + 0.114 * img->b_data[i][j];
            if (i % 2 == 0 && j % 2 == 0) {
                new_img->Cb_data[i / 2][j / 2] = (yCbCr_px) 0.564 * (img->b_data[i][j] - new_img->Y_data[i][j]);
                new_img->Cr_data[i / 2][j / 2] = (yCbCr_px) 0.713 * (img->r_data[i][j] - new_img->Y_data[i][j]);
            }
        }
    }
    return new_img;
}

// Conversion YCrCb -> bmp
bmp_image * yCbCr_to_bmp(YCrCB_image *img) {
    bmp_image* bmp = new_bmp(img->width, img->height);
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            bmp->r_data[i][j] = (px_t)img->Y_data[i][j] + (1.402 * get_Cr(img, i , j)) ;
            bmp->g_data[i][j] = (px_t)img->Y_data[i][j] - (0.344 * get_Cb(img, i , j)) - (0.714 * get_Cr(img, i , j));
            bmp->b_data[i][j] = (px_t)img->Y_data[i][j] + (1.772 * get_Cb(img, i , j));
        }
    }
    return bmp;
}


