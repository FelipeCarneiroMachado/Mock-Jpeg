//
// Created by felipe on 3/31/25.
//

#include "dct.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

//  Pre computed DCT matrix
double dct_coefficient_matrix[8][8];

double idct_coefficient_matrix[8][8];

double compression_factor = 1;

// Checks if its already computed
bool is_dct_computed = false;

int invert_matrix(double A[8][8], double inverse[8][8]);

void compute_dct_coefficient_matrix() {
    is_dct_computed = true;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (i == 0)
                dct_coefficient_matrix[i][j] = 1 / sqrtf(8);
            else
                dct_coefficient_matrix[i][j] = (1 / sqrtf(4)) * cos((2.0 * j + 1) * i * M_PI / 16.0);
        }
    }
    invert_matrix(dct_coefficient_matrix, idct_coefficient_matrix);
}

int invert_matrix(double A[8][8], double inverse[8][8]) {
    // Create augmented matrix [A | I]
    double augmented[8][2*8];

    // Initialize the augmented matrix
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            augmented[i][j] = A[i][j];
            augmented[i][j+8] = (i == j) ? 1.0 : 0.0;
        }
    }

    // Gaussian elimination with partial pivoting
    for (int i = 0; i < 8; i++) {
        // Find the pivot row
        int max_row = i;
        double max_val = fabs(augmented[i][i]);

        for (int k = i + 1; k < 8; k++) {
            if (fabs(augmented[k][i]) > max_val) {
                max_val = fabs(augmented[k][i]);
                max_row = k;
            }
        }

        // Check if matrix is singular
        if (max_val < 0.0001) {
            printf("Matrix is singular or nearly singular\n");
            return 0;
        }

        // Swap rows if needed
        if (max_row != i) {
            for (int j = 0; j < 2*8; j++) {
                double temp = augmented[i][j];
                augmented[i][j] = augmented[max_row][j];
                augmented[max_row][j] = temp;
            }
        }

        // Scale the pivot row
        double pivot = augmented[i][i];
        for (int j = 0; j < 2*8; j++) {
            augmented[i][j] /= pivot;
        }

        // Eliminate other rows
        for (int k = 0; k < 8; k++) {
            if (k != i) {
                double factor = augmented[k][i];
                for (int j = 0; j < 2*8; j++) {
                    augmented[k][j] -= factor * augmented[i][j];
                }
            }
        }
    }

    // Extract the inverse matrix
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            inverse[i][j] = augmented[i][j+8];
        }
    }

    return 1;
}

// Quantization Matrixes

double quantization_Y[8][8] = {
    {16, 11, 10, 16, 24, 40, 51, 61},
    {12, 12, 14, 19, 26, 58, 60, 55},
    {14, 13, 16, 24, 40, 57, 69, 56},
    {14, 17, 22, 29, 51, 87, 80, 62},
    {18, 22, 37, 56, 68, 109, 103, 77},
    {24, 35, 55, 64, 81, 104, 113, 92},
    {79, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

int quantization_Crom[8][8] = {
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}
};


// Sets the compression factor globally
void set_compression_factor(double factor) {
    compression_factor = factor;
}


// DCT on block, works in-place
void apply_block_DCT_8x8(yCbCr_px** src, yCbCr_px** dest, uint32_t y, uint32_t x) {
    if (!is_dct_computed)
         compute_dct_coefficient_matrix();
    yCbCr_px temp_matrix[8][8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            yCbCr_px sum = 0;
            for (int k = 0; k < 8; k++)
                sum += dct_coefficient_matrix[i][k] * src[k + y][j + x];
            temp_matrix[i][j] = sum;
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            yCbCr_px sum = 0;
            for (int k = 0; k < 8; k++)
                sum += temp_matrix[i][k] * dct_coefficient_matrix[j][k];
            dest[i + y][j + x] = sum;
        }
    }
}


// Apply DCT to image, returns an image
dct_YCrCB_image* DCT_8x8(YCrCB_image* img) {

    // Allcoates space for new image
    dct_YCrCB_image* dct = (dct_YCrCB_image*) new_YCrCB_image(img->width, img->height);

    // Iterates through all 8x8 blocks and applies the DCT to the Y component
    for (int i = 0; i < img->height; i += 8)
        for (int j = 0; j < img->width; j += 8)
            apply_block_DCT_8x8(img->Y_data, dct->Y_data, i, j);

    // Iterates through all 8x8 blocks and applies the DCT to the undersampled components
    for (int i = 0; i < img->height / 2; i += 8) {
        for (int j = 0; j < img->width / 2; j += 8) {
            apply_block_DCT_8x8(img->Cr_data, dct->Cr_data, i, j);
            apply_block_DCT_8x8(img->Cb_data, dct->Cb_data, i, j);
        }
    }

    return dct;
}


void apply_block_IDCT_8x8(yCbCr_px** src, yCbCr_px** dest, uint32_t y, uint32_t x) {
    if (!is_dct_computed)
        compute_dct_coefficient_matrix();
    yCbCr_px temp_matrix[8][8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            yCbCr_px sum = 0;
            for (int k = 0; k < 8; k++)
                sum += idct_coefficient_matrix[i][k] * src[k + y][j + x];
            temp_matrix[i][j] = sum;
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            yCbCr_px sum = 0;
            for (int k = 0; k < 8; k++)
                sum += temp_matrix[i][k] * idct_coefficient_matrix[j][k];
            dest[i + y][j + x] = sum;
        }
    }
}

YCrCB_image* IDCT_8x8(dct_YCrCB_image* img) {
    YCrCB_image* reverted = new_YCrCB_image(img->width, img->height);
    // Iterates through all 8x8 blocks and applies the IDCT to the Y component
    for (int i = 0; i < img->height; i += 8)
        for (int j = 0; j < img->width; j += 8)
            apply_block_IDCT_8x8(img->Y_data, reverted->Y_data, i, j);

    // Iterates through all 8x8 blocks and applies the IDCT to the undersampled components
    for (int i = 0; i < img->height / 2; i += 8) {
        for (int j = 0; j < img->width / 2; j += 8) {
            apply_block_IDCT_8x8(img->Cr_data, reverted->Cr_data, i, j);
            apply_block_IDCT_8x8(img->Cb_data, reverted->Cb_data, i, j);
        }
    }
    return reverted;
}


// Utilities for the quantized_dct_image type
// All are self explanatory

quantized_px_t** alloc_quantized_px_t_matrix(int32_t height, int32_t width) {
    quantized_px_t** mat = (quantized_px_t**) malloc(sizeof(quantized_px_t*) * height);
    for (int i = 0; i < height; i++) {
        mat[i] = (quantized_px_t*) malloc(sizeof(quantized_px_t) * width);
    }
    return mat;
}


void free_quantized_px_t_matrix(quantized_px_t*** mat, int32_t height) {
    for (int i = 0; i < height; i++) {
        free((*mat)[i]);
    }
    free(*mat);
    *mat = NULL;
}

quantized_dct_image* new_quantized_dct_image(int32_t height, int32_t width) {
    quantized_dct_image* img  = (quantized_dct_image*)malloc(sizeof(quantized_dct_image));
    img->height = height;
    img->width = width;
    img->Y_quantized = alloc_quantized_px_t_matrix(height, width);
    img->Cr_quantized = alloc_quantized_px_t_matrix(height / 2, width / 2);
    img->Cb_quantized = alloc_quantized_px_t_matrix(height / 2, width / 2);
    return img;
}

// Applies quantization to a single block, set the isYcomponent flag to each kind of quantization matrix
void apply_8x8_block_quantization(yCbCr_px** src, quantized_px_t** dest, int32_t y, int32_t x, bool isYcomponent) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isYcomponent)
                dest[y + i][x + j] = (quantized_px_t)round(src[y + i][x + j] / (compression_factor * quantization_Y[i][j]));
            else
                dest[y + i][x + j] = (quantized_px_t)round(src[y + i][x + j] / (compression_factor * quantization_Crom[i][j]));
        }
    }
}

// Applies quantization to all block
quantized_dct_image* dct_quantization_8x8(dct_YCrCB_image *img) {
    quantized_dct_image* qt = new_quantized_dct_image(img->height, img->width);
    for (int i = 0; i < img->height; i += 8)
        for (int j = 0; j < img->width; j += 8)
            apply_8x8_block_quantization(img->Y_data, qt->Y_quantized, i, j, true);
    for (int i = 0; i < img->height / 2; i += 8) {
        for (int j = 0; j < img->width / 2; j += 8) {
            apply_8x8_block_quantization(img->Cr_data, qt->Cr_quantized, i, j, false);
            apply_8x8_block_quantization(img->Cb_data, qt->Cb_quantized, i, j, false);
        }
    }
    return qt;
}


// Dequantization
void apply_8x8_block_dequantization(quantized_px_t** src, yCbCr_px** dest, int32_t y, int32_t x, bool isYcomponent) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isYcomponent)
                dest[y + i][x + j] = (yCbCr_px) src[y + i][x + j] * compression_factor * quantization_Y[i][j];
            else
                dest[y + i][x + j] = (yCbCr_px) src[y + i][x + j] * compression_factor * quantization_Crom[i][j];
        }
    }
}

dct_YCrCB_image* dequantization_8x8(quantized_dct_image* img) {
    dct_YCrCB_image* dct = (dct_YCrCB_image*) new_YCrCB_image(img->width, img->height);
    for (int i = 0; i < img->height; i += 8)
        for (int j = 0; j < img->width; j += 8)
            apply_8x8_block_dequantization(img->Y_quantized, dct->Y_data, i, j, true);
    for (int i = 0; i < img->height / 2; i += 8) {
        for (int j = 0; j < img->width / 2; j += 8) {
            apply_8x8_block_dequantization(img->Cr_quantized, dct->Cr_data, i, j, false);
            apply_8x8_block_dequantization(img->Cb_quantized, dct->Cb_data, i, j, false);
        }
    }
    return dct;
}




