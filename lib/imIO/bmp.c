//
// Created by felipe on 3/19/25.
//
#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bmp_image* new_bmp_image() {
    bmp_image* bmp = (bmp_image*)malloc(sizeof(bmp_image));
    bmp->header = (bmp_header*)malloc(sizeof(bmp_header));
    bmp->infoHeader = (bmp_info_header*)malloc(sizeof(bmp_info_header));
    if (bmp == NULL || bmp->header == NULL || bmp->infoHeader == NULL) {
        perror("Alocation error.\n");
        return NULL;
    }
    return bmp;
}

px_t** px_matrix_alloc(uint32_t height, uint32_t width) {
    px_t** matrix = (px_t**)malloc(sizeof(px_t*) * width);
    for (int i = 0; i < width; i++) {
        matrix[i] = (px_t*)malloc(sizeof(px_t) * height);
    }
    return matrix;
}

void bmp_free(bmp_image** bmp) {
    if ((*bmp)->header != NULL) {
        free((*bmp)->header);
    }
    if ((*bmp)->infoHeader != NULL) {
        free((*bmp)->infoHeader);

    }
    if ((*bmp)->r_data) {
        free((*bmp)->b_data);
    }
    if ((*bmp)->g_data) {
        free((*bmp)->b_data);
    }
    if ((*bmp)->b_data) {
        free((*bmp)->b_data);
    }
    free(*bmp);
}

bmp_image* bmp_read(const char* filename) {

    FILE* fd = fopen(filename, "r");
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
    if (bmp->header->signature[0] != 'B' || bmp->header->signature[1] != 'M') {
        perror("Unsuported BMP file format\n");
        bmp_free(&bmp);
        fclose(fd);
        return  NULL;
    }

    // File size
    memcpy(&bmp->header->filesize, headerBuffer + 2, 4);

    // Pixel data offset
    memcpy(&bmp->header->dataOffset, headerBuffer + 10, 4);

    // Reading and converting to struct the first header

    char infoHeaderBuffer[40];
    fread(infoHeaderBuffer, 40, 1, fd);

    // Width and heigth
    memcpy(&bmp->infoHeader->width, headerBuffer + 4, 4);
    memcpy(&bmp->infoHeader->height, headerBuffer + 8, 4);

    bmp->r_data = px_matrix_alloc(bmp->infoHeader->height, bmp->infoHeader->width);
    bmp->g_data = px_matrix_alloc(bmp->infoHeader->height, bmp->infoHeader->width);
    bmp->b_data = px_matrix_alloc(bmp->infoHeader->height, bmp->infoHeader->width);








}


