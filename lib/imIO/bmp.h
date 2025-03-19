//
// Created by felipe on 3/19/25.
//

#ifndef BMP_H
#define BMP_H
#include <stdint.h>

    typedef unsigned char px_t;

    typedef struct {
        char signature[2];
        uint32_t filesize;
        uint32_t dataOffset;
    } bmp_header;

    typedef struct {
        uint32_t width;
        uint32_t height;
        // TODO: add the other fields (ONLY IF NECESSARY)
    } bmp_info_header;

    typedef struct {
        bmp_header* header;
        bmp_info_header* infoHeader;
        px_t** r_data;
        px_t** g_data;
        px_t** b_data;
    } bmp_image;


    bmp_image* bmp_read(const char* filename);

    int bmp_save(bmp_image* image, const char* filename);

    void bmp_free(bmp_image** bmp);




#endif //BMP_H
