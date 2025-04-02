//
// Created by felipe on 3/19/25.
//

/*
 * .bmp image input/output + in-code representation
 */
#ifndef BMP_H
#define BMP_H
#include <stdint.h>

    // 8-bit pixel type
    typedef unsigned char px_t;

    // BMP main header
    typedef struct {
        char signature[2];
        uint32_t filesize;
        uint32_t dataOffset;
    } bmp_header;

    // BMP info header
    typedef struct {
        uint32_t headerSize;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bitsPerPixel;
        uint32_t compression;
        uint32_t imageSize;
        uint32_t horizontalResolution;
        uint32_t verticalResolution;
        uint32_t numColors;
        uint32_t colorsUsed;
    } bmp_info_header;

    // BMP image full info
    typedef struct {
        bmp_header* header;
        bmp_info_header* infoHeader;
        px_t** r_data;
        px_t** g_data;
        px_t** b_data;
    } bmp_image;

    // Loading from filename
    bmp_image* bmp_read(const char* filename);

    // Saves to a file from a struct
    int bmp_save(bmp_image* image, const char* filename);

    // Memory deallocation for bmp struct
    void bmp_free(bmp_image** bmp);

    // TODO: printing all info on headers for debug (AUGUSTO)
    void print_bmp_info(bmp_image *bmp);

    // Create bmp_image with initial info
    bmp_image* new_bmp(int32_t width, int32_t height);


#endif //BMP_H
