//
// Created by felipe on 3/31/25.
//

#include "dct.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

//  Pre computed DCT matrix
double dct_coefficient_matrix[8][8];

// Checks if its already computed
bool is_dct_computed = false;

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
}


// Quantization Matrixes

int quantization_Y[8][8] = {
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


void apply_block_DCT_8x8(yCbCr_px** block, yCbCr_px** dest) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            yCbCr_px sum = 0;
            for (int k = 0; k < 8; k++) {
                for (int l = 0; l < 8; l++) {
                    // sum += dct_coefficient_matrix[]
                }
            }

        }
    }
}



YCrCB_image* DCT_8x8(YCrCB_image* img) {
    YCrCB_image* dct = newYCrCB_image(img->width, img->height);
    for (int i = 0; i < img->width; i += 8) {
        for (int j = 0; j < img->height; j += 8) {

        }
    }
}









