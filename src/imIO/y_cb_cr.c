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

// Initialization and alllocation for images
YCrCB_image* new_YCrCB_image(int32_t width, int32_t height) {
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

// Computes Cb average for subsampling
yCbCr_px cb_block_avg(bmp_image* img, int32_t y, int32_t x) {
    yCbCr_px coefs[4];
    coefs[0] = (yCbCr_px) 128.0 + (-0.169 * img->r_data[y][x] - 0.331 * img->g_data[y][x] + 0.50 * img->b_data[y][x]);
    coefs[1] = (yCbCr_px) 128.0 + (-0.169 * img->r_data[y+1][x] - 0.331 * img->g_data[y+1][x] + 0.50 * img->b_data[y+1][x]);
    coefs[2] = (yCbCr_px) 128.0 + (-0.169 * img->r_data[y][x+1] - 0.331 * img->g_data[y][x+1] + 0.50 * img->b_data[y][x+1]);
    coefs[3] = (yCbCr_px) 128.0 + (-0.169 * img->r_data[y+1][x+1] - 0.331 * img->g_data[y+1][x+1] + 0.50 * img->b_data[y+1][x+1]);
    return (yCbCr_px)(coefs[0] + coefs[1] + coefs[2] + coefs[3]) / 4.0;
}

// Computes Cr average for subsampling
yCbCr_px cr_block_avg(bmp_image* img, int32_t y, int32_t x) {
    yCbCr_px coefs[4];
    coefs[0] = (yCbCr_px) 128.0 + (0.5 * img->r_data[y][x] - 0.419 * img->g_data[y][x] - 0.081 * img->b_data[y][x]);
    coefs[1] = (yCbCr_px) 128.0 + (0.5 * img->r_data[y+1][x] - 0.419 * img->g_data[y+1][x] - 0.081 * img->b_data[y+1][x]);
    coefs[2] = (yCbCr_px) 128.0 + (0.5 * img->r_data[y][x+1] - 0.419 * img->g_data[y][x+1] - 0.081 * img->b_data[y][x+1]);
    coefs[3] = (yCbCr_px) 128.0 + (0.5 * img->r_data[y+1][x+1] - 0.419 * img->g_data[y+1][x+1] - 0.081 * img->b_data[y+1][x+1]);
    return (yCbCr_px)(coefs[0] + coefs[1] + coefs[2] + coefs[3]) / 4.0;
}


// Conversion bmp -> YCrCb
YCrCB_image* bmp_to_yCbCr(bmp_image* img) {
    YCrCB_image* new_img = new_YCrCB_image(img->infoHeader->width, img->infoHeader->height);
    for (int i = 0; i < img->infoHeader->height; i++) {
        for (int j = 0; j < img->infoHeader->width; j++) {
            new_img->Y_data[i][j] = (yCbCr_px) 0.299 * img->r_data[i][j] + 0.587 * img->g_data[i][j] + 0.114 * img->b_data[i][j];
            if (i % 2 == 0 && j % 2 == 0) {
                new_img->Cb_data[i / 2][j / 2] = cb_block_avg(img, i, j);
                new_img->Cr_data[i / 2][j / 2] = cr_block_avg(img, i, j);
            }
        }
    }
    return new_img;
}

// Clampls float value to 0-255 uint8 interval (avoids over/underflows)
px_t clamp_interval(yCbCr_px value) {
    return value < 0 ? 0 : (value > 255 ? 255 : (px_t) value);
}


// Conversion YCrCb -> bmp
bmp_image * yCbCr_to_bmp(YCrCB_image *img) {
    bmp_image* bmp = new_bmp(img->width, img->height);
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            bmp->r_data[i][j] = clamp_interval(img->Y_data[i][j] + (1.402 * (get_Cr(img, i , j) - 128.0)));
            bmp->g_data[i][j] = clamp_interval(img->Y_data[i][j] - (0.344 * (get_Cb(img, i , j) - 128.0)) - (0.714 * (get_Cr(img, i , j) - 128)));
            bmp->b_data[i][j] = clamp_interval(img->Y_data[i][j] + (1.772 * (get_Cb(img, i , j) - 128.0)));
        }
    }
    return bmp;
}


