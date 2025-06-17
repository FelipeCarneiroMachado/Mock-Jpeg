//
// Created by felipe on 3/19/25.
//
#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Empty bmp allcoation and creation
bmp_image* new_bmp_image() {
    bmp_image* bmp = (bmp_image*)malloc(sizeof(bmp_image));
    if (bmp == NULL) {
        perror("Alocation error.\n");
        return NULL;
    }
    bmp->header = (bmp_header*)malloc(sizeof(bmp_header));
    bmp->infoHeader = (bmp_info_header*)malloc(sizeof(bmp_info_header));
    if (bmp->header == NULL || bmp->infoHeader == NULL) {
        perror("Alocation error.\n");
        return NULL;
    }
    return bmp;
}
// Memory deallocation for pixel data
void free_px_matrix(px_t ***matrix, uint32_t nrows, uint32_t ncols) {
    for (uint32_t i = 0; i < nrows; i++) {
        free((*matrix)[i]);
    }
    free(*matrix);
    *(matrix) = NULL;
}

// Memory eallocation for pixel data
px_t** px_matrix_alloc(uint32_t height, uint32_t width) {
    px_t** matrix = (px_t**)malloc(sizeof(px_t*) * height);
    for (int i = 0; i < height; i++) {
        matrix[i] = (px_t*)malloc(sizeof(px_t) * width);
    }
    return matrix;
}

// Memory deallocation for bmp struct
void bmp_free(bmp_image** bmp) {

    if ((*bmp)->r_data != NULL) {
        free_px_matrix(&((*bmp)->r_data), (*bmp)->infoHeader->height, (*bmp)->infoHeader->width);
    }
    if ((*bmp)->g_data != NULL) {
        free_px_matrix(&((*bmp)->g_data), (*bmp)->infoHeader->height, (*bmp)->infoHeader->width);
    }
    if ((*bmp)->b_data != NULL) {
        free_px_matrix(&((*bmp)->b_data), (*bmp)->infoHeader->height, (*bmp)->infoHeader->width);
    }
    if ((*bmp)->header != NULL) {
        free((*bmp)->header);
    }
    if ((*bmp)->infoHeader != NULL) {
        free((*bmp)->infoHeader);

    }
    free(*bmp);
}

// Bmp allocation with initial size info
bmp_image * new_bmp(int32_t width, int32_t height) {
    bmp_image* im = new_bmp_image();
    // Main header setup
    im->header->signature[0] = 'B';
    im->header->signature[1] = 'M';
    im->header->filesize = 54 + (width * height * 3);
    im->header->dataOffset = 54;
    // Info Header setup
    im->infoHeader->headerSize = 40;
    im->infoHeader->width = width;
    im->infoHeader->height = height;
    im->infoHeader->planes = 1;
    im->infoHeader->bitsPerPixel = 24;
    im->infoHeader->compression = 0;
    im->infoHeader->imageSize = 0;
    im->infoHeader->horizontalResolution = 0;
    im->infoHeader->verticalResolution = 0;
    im->infoHeader->numColors = 0;
    im->infoHeader->colorsUsed = 16777216;

    // Pixel data setup
    im->r_data = px_matrix_alloc(height, width);
    im->g_data = px_matrix_alloc(height, width);
    im->b_data = px_matrix_alloc(height, width);

    return im;
}

// Validates if the bmp read fits into the project specification
bool validate_bmp(bmp_image* bmp) {
    bool flag = true;
    // BM file
    if (bmp->header->signature[0] != 'B' || bmp->header->signature[1] != 'M') {
        flag = false;
    }
    // Dimensions divisible by 8
    if (bmp->infoHeader->width % 8 != 0 || bmp->infoHeader->height % 8 != 0) {
        flag = false;
    }
    // 1 plane
    if (bmp->infoHeader->planes != 1) {
        flag = false;
    }
    // No compression
    if (bmp->infoHeader->compression != 0) {
        flag = false;
    }
    // 24 px RGB
    if (bmp->infoHeader->bitsPerPixel != 24) {
        flag = false;
    }
    // 40 byte header
    if (bmp->infoHeader->headerSize != 40) {
        flag = false;
    }
    return flag;
}

// Reads bmp from a file
bmp_image* bmp_read(const char* filename) {

    FILE* fd = fopen(filename, "rb");
    if (fd == NULL) {
        perror("Error opening file\n");
        return NULL;
    }

    // Allocation
    bmp_image* bmp = new_bmp_image();
    if (bmp == NULL) {
        bmp_free(&bmp);
        return NULL;
    }

    // Reading and converting to struct the first header

    char headerBuffer[14];
    fread(headerBuffer, 14, 1, fd);

    // File signature
    memcpy(bmp->header->signature, headerBuffer, 2);

    // File size
    memcpy(&bmp->header->filesize, headerBuffer + 2, 4);

    // Pixel data offset
    memcpy(&bmp->header->dataOffset, headerBuffer + 10, 4);

    // Reading and converting to struct the info header

    char infoHeaderBuffer[40];
    fread(infoHeaderBuffer, 40, 1, fd);

    // Header Size
    memcpy((&bmp->infoHeader->headerSize), infoHeaderBuffer, 4);

    // Width and heigth
    memcpy(&bmp->infoHeader->width, infoHeaderBuffer + 4, 4);
    memcpy(&bmp->infoHeader->height, infoHeaderBuffer + 8, 4);

    // Planes
    memcpy(&bmp->infoHeader->planes, infoHeaderBuffer + 12, 2);
    // Bits per pixel
    memcpy(&bmp->infoHeader->bitsPerPixel, infoHeaderBuffer + 14, 2);
    // Compression
    memcpy(&bmp->infoHeader->compression, infoHeaderBuffer + 16, 4);
    // Image size
    memcpy(&bmp->infoHeader->imageSize, infoHeaderBuffer + 20, 4);
    // Resolution
    memcpy(&bmp->infoHeader->horizontalResolution, infoHeaderBuffer + 24, 4);
    memcpy(&bmp->infoHeader->verticalResolution, infoHeaderBuffer + 28, 4);
    // Color number
    memcpy(&bmp->infoHeader->numColors, infoHeaderBuffer + 32, 4);
    memcpy(&bmp->infoHeader->colorsUsed, infoHeaderBuffer + 36, 4);

    // Validate if the bmp is following the necessary format
    if (!validate_bmp(bmp)) {
        bmp_free(&bmp);
        fclose(fd);
        perror("BMP file doesnt follow specifications.\n");
        return NULL;
    }

    // Pixel data reading
    int32_t h_padding = 0;
    int32_t v_padding = 0;
    if (bmp->infoHeader->width % 16 != 0)
        h_padding = 16 - bmp->infoHeader->width % 16;
    if (bmp->infoHeader->height % 16 != 0)
        v_padding = 16 - bmp->infoHeader->height % 16;

    // Allocation
    bmp->r_data = px_matrix_alloc(bmp->infoHeader->height + v_padding, bmp->infoHeader->width + h_padding);
    bmp->g_data = px_matrix_alloc(bmp->infoHeader->height + v_padding, bmp->infoHeader->width + h_padding);
    bmp->b_data = px_matrix_alloc(bmp->infoHeader->height + v_padding, bmp->infoHeader->width + h_padding);

    fseek(fd, bmp->header->dataOffset, SEEK_SET);

    // Iterating through file, rows are bottom on file
    for (int32_t i = (int32_t)bmp->infoHeader->height - 1 + v_padding; i >= 0; i--) {
        for (uint32_t j = 0; j < bmp->infoHeader->width + h_padding; j++) {
            if (j >= bmp->infoHeader->width || i >= bmp->infoHeader->height) {
                bmp->b_data[i][j] = 0;
                bmp->g_data[i][j] = 0;
                bmp->r_data[i][j] = 0;
            }
            else {
                fread(&bmp->b_data[i][j], 1, 1, fd);
                fread(&bmp->g_data[i][j], 1, 1, fd);
                fread(&bmp->r_data[i][j], 1, 1, fd);
            }
        }
    }
    bmp->infoHeader->height += v_padding;
    bmp->infoHeader->width += h_padding;
    return bmp;
}

// Saves bmp to a file
int bmp_save(bmp_image *bmp, const char *filename) {
    FILE* out = fopen(filename, "wb");
    if (out == NULL) {
        perror("Error opening file\n");
        return -1;
    }

    // Main header

    // Signature
    fwrite(bmp->header->signature, 1, 2, out);
    // File size
    fwrite(&bmp->header->filesize, 4, 1, out);
    // Reserved
    int zero = 0;
    fwrite(&zero, 1, 4, out);
    // Data offset
    fwrite(&bmp->header->dataOffset, 4, 1, out);

    // Info header

    // Info header size
    fwrite(&bmp->infoHeader->headerSize, 4, 1, out);
    // Dimensions
    fwrite(&bmp->infoHeader->width, 4, 1, out);
    fwrite(&bmp->infoHeader->height, 4, 1, out);
    // Other info
    fwrite(&bmp->infoHeader->planes, 2, 1, out);
    fwrite(&bmp->infoHeader->bitsPerPixel, 2, 1, out);
    fwrite(&bmp->infoHeader->compression, 4, 1, out);
    fwrite(&bmp->infoHeader->imageSize, 4, 1, out);
    fwrite(&bmp->infoHeader->horizontalResolution, 4, 1, out);
    fwrite(&bmp->infoHeader->verticalResolution, 4, 1, out);
    fwrite(&bmp->infoHeader->numColors, 4, 1, out);
    fwrite(&bmp->infoHeader->colorsUsed, 4, 1, out);

    // Pixel writing
    for (int32_t i = (int32_t)bmp->infoHeader->height - 1; i >= 0; i--) {
        for (uint32_t j = 0; j < bmp->infoHeader->width; j++) {
            fwrite(&bmp->b_data[i][j], 1, 1, out);
            fwrite(&bmp->g_data[i][j], 1, 1, out);
            fwrite(&bmp->r_data[i][j], 1, 1, out);
        }
    }

    return 1;

}




