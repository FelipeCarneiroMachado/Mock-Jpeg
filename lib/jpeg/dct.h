//
// Created by felipe on 3/31/25.
//

/*
 * Implements the Discrete Cosine Transform (DCT) for the JPEG compression
 * Uses YCrCb images (defined in y_cr_cb.h) as input
 */

#ifndef DCT_H
#define DCT_H
#include "../imIO/y_cb_cr.h"








    YCrCB_image* DCT_8x8(YCrCB_image* img);

#endif //DCT_H
