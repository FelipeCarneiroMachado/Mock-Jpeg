#include "diff_rle.h"
#include <stdlib.h>


//allocates a matrix of pointers to arrays of 63 pairs of ints for the AC components
pair (**alloc_AC_matrix(int32_t height, int32_t width))[63] {
    pair (**mat)[63];
    mat = malloc(height * sizeof(pair (*)[63]));
    for (int32_t i = 0; i < height; i++) {
        mat[i] = malloc(width * sizeof(pair [63]));
    }
    return mat;
}


//allocates a matrix of pointers to ints for the DC components
int32_t **alloc_DC_matrix(int32_t height, int32_t width){
    int32_t  **mat;
    mat = malloc(height * sizeof(int32_t*));
    for (int32_t i = 0; i < height; i++) {
        mat[i] = malloc(width * sizeof(int32_t));
    }
    return mat;

}

//frees the matrix for the AC components
void AC_matrix_free(pair (**mat)[63], int32_t height) {
    for (int32_t i = 0; i < height; i++) {
        free(mat[i]);
    }
    free(mat);
}


//frees the matrix for the DC components
void DC_matrix_free(int32_t** mat, int32_t height) {
    for (int32_t i = 0; i < height; i++) {
        free(mat[i]);
    }
    free(mat);
}

//allocates the struct that holds the image after it's been runlenght and diffrerence encoded
rlediff_img *rlediff_img_alloc(int32_t height, int32_t width, float compression_factor){
    rlediff_img* rlediff = (rlediff_img*)malloc(sizeof(rlediff_img));
    rlediff->height = height;
    rlediff->width = width;
    rlediff->compression_factor = compression_factor;

    //allocates the DC matrixes
    rlediff->Y_block_DC = alloc_DC_matrix(height, width);
    rlediff->Cb_block_DC = alloc_DC_matrix(height / 2, width / 2);
    rlediff->Cr_block_DC = alloc_DC_matrix(height / 2, width / 2);

    //allocates the AC matrixes
    rlediff->Y_block_AC = alloc_AC_matrix(height, width);
    rlediff->Cb_block_AC = alloc_AC_matrix(height / 2, width / 2);
    rlediff->Cr_block_AC = alloc_AC_matrix(height / 2, width / 2);

    return rlediff;
}

//frees the struct that holds the image after it's been runlenght and diffrerence encoded
void rlediff_img_free(rlediff_img** img) {

    //frees the AC matrixes
    AC_matrix_free((*img)->Y_block_AC, (*img)->height);
    AC_matrix_free((*img)->Cb_block_AC, (*img)->height / 2);
    AC_matrix_free((*img)->Cr_block_AC, (*img)->height / 2);

    //frees the DC matrixes
    DC_matrix_free((*img)->Y_block_DC, (*img)->height);
    DC_matrix_free((*img)->Cb_block_DC, (*img)->height / 2);
    DC_matrix_free((*img)->Cr_block_DC, (*img)->height / 2);
    free(*img);
    *img = NULL;
}


//difference encodes a matrix of DC components from a matrix of vectorized blocks
void DC_encode(int32_t **block_DC, int32_t (**block_vectorized)[64], int32_t height, int32_t width){
    //gets the AC component (first element of the vectorized block)
    block_DC[0][0] = block_vectorized[0][0][0];
    for(int32_t i = 0;i<height;i++){
        if(i!=0){
            //the first component of each row is the differenc betwwen this and the one from the row above
            //except for the very first
            block_DC[i][0] = block_vectorized[i][0][0] - block_vectorized[i-1][0][0];
        }
        //each component after the first from each row is set to the differece between it and the component left
        for(int32_t j = 1;j<width;j++){
            block_DC[i][j] = block_vectorized[i][j][0] - block_vectorized[i][j-1][0];
        }
    }

}

//diffence decodes a matrix of DC components and fills the first elements of a matrix of vectorized blocks
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

//runlenght encodes a matrix of AC components from a matrix of vectorized blocks
void AC_encode(pair (**block_AC)[63], int32_t (**block_vectorized)[64], int32_t height, int32_t width){
   for(int32_t i = 0; i < height; i++) {
        for(int32_t j = 0; j < width; j++) {
            int32_t num = 1; 
            int32_t k = 0;   

            while(num < 64 && k < 63) {
                if(block_vectorized[i][j][num] == 0) {
                    //counts how many zeroes there are in until the next non zero number
                    int32_t count = 0;
                    while(num < 64 && block_vectorized[i][j][num] == 0) {
                        count++;
                        num++;
                        if(count==16){
                            //if there are 16 sequential zeros saves it as (16,0) for huffman reasons
                            block_AC[i][j][k].first = 15;
                            block_AC[i][j][k].second = 0;
                            count = 0;
                            k++;
                        }
                    }

                    if(num == 64) {
                        //if it reaches the end of the vectorized block it saves a (0,0)
                        block_AC[i][j][k].first = 0;
                        block_AC[i][j][k].second = 0;
                        break;
                    }
                    //saves the pair of (count, value)
                    block_AC[i][j][k].first = count;
                    block_AC[i][j][k].second = block_vectorized[i][j][num];
                    num++;
                    k++;
                } else {
                    //if there were no zeros in between saves the pair (0,value)
                    block_AC[i][j][k].first = 0;
                    block_AC[i][j][k].second = block_vectorized[i][j][num];
                    num++;
                    k++;
                }
            }
            if(k < 63) {
                //condition for if the last number is not zero
                block_AC[i][j][k].first = 0;
                block_AC[i][j][k].second = 0;
            }
        }
    }

}

//allocates and fills the struct for the image after its been difference and runlength encoded
rlediff_img *partial_encode(vectorized_img *vectorized_img){
    //allocates the struct
    int32_t height = vectorized_img->height;
    int32_t width = vectorized_img->width;
    float compression_factor = get_compression_factor();
    rlediff_img *img = rlediff_img_alloc(height,width,compression_factor);

    //fills the DC matrixes
    DC_encode(img->Y_block_DC,vectorized_img->Y_block_arrays,height,width);
    DC_encode(img->Cb_block_DC,vectorized_img->Cb_block_arrays,height/2,width/2);
    DC_encode(img->Cr_block_DC,vectorized_img->Cr_block_arrays,height/2,width/2);

    //fills the AC matrixes
    AC_encode(img->Y_block_AC,vectorized_img->Y_block_arrays,height,width);
    AC_encode(img->Cb_block_AC,vectorized_img->Cb_block_arrays,height/2,width/2);
    AC_encode(img->Cr_block_AC,vectorized_img->Cr_block_arrays,height/2,width/2);

    return img;

}


//runlenght decodes a matrix of AC components into a matrix of vectorized blocks
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


//runlenght and difference decodes the image into a struct that stores the image vectorized blocks
vectorized_img *partial_decode(rlediff_img *rlediff_img){
    //allocates the vectorized_img struct
    int32_t height = rlediff_img->height;
    int32_t width = rlediff_img->width;
    vectorized_img *img = vectorized_img_alloc(height,width);


    //fills the DC components(first elements of the vectors)
    DC_decode(rlediff_img->Y_block_DC,img->Y_block_arrays,height,width);
    DC_decode(rlediff_img->Cb_block_DC,img->Cb_block_arrays,height/2,width/2);
    DC_decode(rlediff_img->Cr_block_DC,img->Cr_block_arrays,height/2,width/2);

    //fills the AC components(other elements of the vector)
    AC_decode(rlediff_img->Y_block_AC,img->Y_block_arrays,height,width);
    AC_decode(rlediff_img->Cb_block_AC,img->Cb_block_arrays,height/2,width/2);
    AC_decode(rlediff_img->Cr_block_AC,img->Cr_block_arrays,height/2,width/2);

    return img;

}