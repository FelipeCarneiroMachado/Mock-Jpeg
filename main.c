#include <stdio.h>
#include "lib/imIO/bmp.h"
#include <stdlib.h>
#include "lib/imIO/y_cb_cr.h"
#include "lib/jpeg/dct.h"
#include "lib/jpeg/vectorization.h"
#include "lib/jpeg/diff_rle.h"
#include "lib/jpeg/huffman.h"

/*void setDC(int32_t (**aarrays)[64], int32_t (**barrays)[64],int32_t height, int32_t width){
    for(int32_t i = 0;i<height;i++){
        for(int32_t j = 0;j<width;j++){
            barrays[i][j][0] = aarrays[i][j][0];
        }
    }
}
    
void setallDC(vectorized_img* a, vectorized_img* b){
    uint32_t height = a->height;
    uint32_t width = a->width;
    setDC(a->Y_block_arrays,b->Y_block_arrays,height,width);
    setDC(a->Cb_block_arrays,b->Cb_block_arrays,height/2,width/2);
    setDC(a->Cr_block_arrays,b->Cr_block_arrays,height/2,width/2);
}*/


void printDCY(rlediff_img *img){
    for(int32_t i = 0;i<img->height;i++){
        for(int32_t j = 0;j<img->width;j++){
            printf("%d ",(img->Y_block_DC)[i][j]);
        }
        printf("\n");
    }
}

void printAC2( pair (**aarrays)[63], int height, int width){
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("comp:\n");
            for(int32_t k = 0;k<63;k++){
                    printf("(%d,%d) ",aarrays[i][j][k].first,aarrays[i][j][k].second);
                    if(aarrays[i][j][k].first== 0 && aarrays[i][j][k].second == 0){
                        break;
                    }
            }
            printf("\n");
        }
    }
}




int main(int argc, char *argv[]) {
    bmp_image* bmp = bmp_read("assets/snail.bmp");
    if (bmp == NULL) {
        perror("Error reading file\n");
        return 1;
    }

    set_compression_factor(strtod(argv[1], NULL));
    YCrCB_image* ycc = bmp_to_yCbCr(bmp);
    dct_YCrCB_image* dcted= DCT_8x8(ycc);
    // int32_t y = 800, x = 640;
    // print_dct_block(dcted, y, x, true, false, false);
    quantized_dct_image* quantized= dct_quantization_8x8(dcted);
    // print_quantized_block(quantized, y, x, true, false, false);
    vectorized_img* vec = vectorize_quantized_img(quantized);
    rlediff_img* part_img = partial_encode(vec);
    huffman_encode(part_img,"huffimg.bin");
    rlediff_img* part_after = huffman_decode("huffimg.bin");
    printAC2(part_img->Y_block_AC,part_img->height,part_img->width);
    vectorized_img* vec_after = partial_decode(part_img);
    quantized_dct_image* qt_after = devectorize_img(vec);
    dct_YCrCB_image* dequantized = dequantization_8x8(qt_after);
    // print_dct_block(dequantized, y, x, true, false, false);

    YCrCB_image* idcted = IDCT_8x8(dequantized);
    


    bmp_image* bmp2 = yCbCr_to_bmp(idcted);
    // printf("%u %u", bmp->infoHeader->horizontalResolution, bmp->infoHeader->verticalResolution);
    bmp_save(bmp2, "rebuilt.bmp");
}