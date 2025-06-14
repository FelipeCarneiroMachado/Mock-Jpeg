#include "diff_rle.h"
#include <stdlib.h>

pair (**alloc_AC_matrix(int32_t height, int32_t width))[63] {
    pair (**mat)[64];
    mat = malloc(height * sizeof(pair (*)[63]));
    for (int32_t i = 0; i < height; i++) {
        mat[i] = malloc(width * sizeof(pair [63]));
    }
    return mat;
}

int32_t **alloc_DC_matrix(int32_t height, int32_t width){
    int32_t  **mat;
    mat = malloc(height * sizeof(int32_t*));
    for (int32_t i = 0; i < height; i++) {
        mat[i] = malloc(width * sizeof(int32_t));
    }
    return mat;

}

void AC_matrix_free(pair (**mat)[64], int32_t height, int32_t width) {
    for (int32_t i = 0; i < height; i++) {
        free(mat[i]);
    }
    free(mat);
}

void DC_matrix_free(pair (**mat)[64], int32_t height, int32_t width) {
    for (int32_t i = 0; i < height; i++) {
        free(mat[i]);
    }
    free(mat);
}


rlediff_img *rlediff_img_alloc(int32_t height, int32_t width){
    rlediff_img* rlediff = (rlediff_img*)malloc(sizeof(rlediff_img));
    rlediff->height = height;
    rlediff->width = width;

    rlediff->Y_block_DC = alloc_DC_matrix(height, width);
    rlediff->Cb_block_DC = alloc_DC_matrix(height / 2, width / 2);
    rlediff->Cr_block_DC = alloc_DC_matrix(height / 2, width / 2);

    rlediff->Y_block_AC = alloc_AC_matrix(height, width);
    rlediff->Cb_block_AC = alloc_AC_matrix(height / 2, width / 2);
    rlediff->Cr_block_AC = alloc_AC_matrix(height / 2, width / 2);

    return rlediff;
}


void rlediff_img_free(rlediff_img** img) {
    AC_matrix_free((*img)->Y_block_AC, (*img)->height, (*img)->width);
    AC_matrix_free((*img)->Cb_block_AC, (*img)->height / 2, (*img)->width / 2);
    AC_matrix_free((*img)->Cr_block_AC, (*img)->height / 2, (*img)->width / 2);
    DC_matrix_free((*img)->Y_block_DC, (*img)->height, (*img)->width);
    DC_matrix_free((*img)->Cb_block_DC, (*img)->height / 2, (*img)->width / 2);
    DC_matrix_free((*img)->Cr_block_DC, (*img)->height / 2, (*img)->width / 2);
    free(*img);
    *img = NULL;
}

void DC_encode(int32_t **block_DC, int32_t (**block_vectorized)[64], int32_t height, int32_t width){
    block_DC[0][0] = block_vectorized[0][0][0];
    for(int32_t i = 0;i<height;i++){
        if(i!=0){
            block_DC[i][0] = block_vectorized[i][0][0] - block_vectorized[i-1][0][0];
        }
        for(int32_t j = 1;j<width;j++){
            block_DC[i][j] = block_vectorized[i][j][0] - block_vectorized[i][j-1][0];
        }
    }

}

void DC_decode(int32_t **block_DC, int32_t (**block_vectorized)[64], int32_t height, int32_t width){
    block_vectorized[0][0][0] = block_DC[0][0];
    for(int32_t i = 0;i<height;i++){
        if(i!=0){
            block_vectorized[i][0][0] = block_vectorized[i-1][0][0] + block_DC[i][0];
        }
        for(int32_t j = 1;j<width;j++){
             block_vectorized[i][j][0] = block_vectorized[i][j-1][0] + block_DC[i][j];
        }
    }
}

void AC_encode(pair (**block_AC)[63], int32_t (**block_vectorized)[64], int32_t height, int32_t width){
   for(int32_t i = 0; i < height; i++) {
        for(int32_t j = 0; j < width; j++) {
            int32_t num = 1; 
            int32_t k = 0;   

            while(num < 64 && k < 63) {
                if(block_vectorized[i][j][num] == 0) {
                    int32_t count = 0;
                    while(num < 64 && block_vectorized[i][j][num] == 0) {
                        count++;
                        num++;
                        if(count==16){
                            block_AC[i][j][k].first = 15;
                            block_AC[i][j][k].second = 0;
                            count = 0;
                            k++;
                        }
                    }

                    if(num == 64) {
                        block_AC[i][j][k].first = 0;
                        block_AC[i][j][k].second = 0;
                        break;
                    }
                    block_AC[i][j][k].first = count;
                    block_AC[i][j][k].second = block_vectorized[i][j][num];
                    num++;
                    k++;
                } else {
                    block_AC[i][j][k].first = 0;
                    block_AC[i][j][k].second = block_vectorized[i][j][num];
                    num++;
                    k++;
                }
            }
            if(k < 63) {
                block_AC[i][j][k].first = 0;
                block_AC[i][j][k].second = 0;
            }
        }
    }

}

rlediff_img *partial_encode(vectorized_img *vectorized_img){
    int32_t height = vectorized_img->height;
    int32_t width = vectorized_img->width;
    rlediff_img *img = rlediff_img_alloc(height,width);

    DC_encode(img->Y_block_DC,vectorized_img->Y_block_arrays,height,width);
    DC_encode(img->Cb_block_DC,vectorized_img->Cb_block_arrays,height/2,width/2);
    DC_encode(img->Cr_block_DC,vectorized_img->Cr_block_arrays,height/2,width/2);

    AC_encode(img->Y_block_AC,vectorized_img->Y_block_arrays,height,width);
    AC_encode(img->Cb_block_AC,vectorized_img->Cb_block_arrays,height/2,width/2);
    AC_encode(img->Cr_block_AC,vectorized_img->Cr_block_arrays,height/2,width/2);

    return img;

}

void AC_decode(pair (**block_AC)[63], int32_t (**block_vectorized)[64], int32_t height, int32_t width){
    for (int32_t i = 0; i < height; i++) {
        for (int32_t j = 0; j < width; j++) {
            int32_t pos = 1; 
            int32_t k = 0;   

            while (k < 63) {
                int32_t run = block_AC[i][j][k].first;
                int32_t value = block_AC[i][j][k].second;


                if (run == 0 && value == 0) {
                    while (pos < 64) {
                        block_vectorized[i][j][pos] = 0;
                        pos++;
                    }
                    break;
                }


                for (int32_t z = 0; z < run; z++) {
                    if (pos < 64) {
                        block_vectorized[i][j][pos] = 0;
                        pos++;
                    }
                }


                if (pos < 64) {
                    block_vectorized[i][j][pos] = value;
                    pos++;
                }

                k++;
            }


            while (pos < 64) {
                block_vectorized[i][j][pos] = 0;
                pos++;
            }
        }
    }
}


vectorized_img *partial_decode(rlediff_img *rlediff_img){
    int32_t height = rlediff_img->height;
    int32_t width = rlediff_img->width;
   vectorized_img *img = vectorized_img_alloc(height,width);

    DC_decode(rlediff_img->Y_block_DC,img->Y_block_arrays,height,width);
    DC_decode(rlediff_img->Cb_block_DC,img->Cb_block_arrays,height/2,width/2);
    DC_decode(rlediff_img->Cr_block_DC,img->Cr_block_arrays,height/2,width/2);

    AC_decode(rlediff_img->Y_block_AC,img->Y_block_arrays,height,width);
    AC_decode(rlediff_img->Cb_block_AC,img->Cb_block_arrays,height/2,width/2);
    AC_decode(rlediff_img->Cr_block_AC,img->Cr_block_arrays,height/2,width/2);

    return img;

}