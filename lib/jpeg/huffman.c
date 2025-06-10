#include<stdio.h>
#include<math.h>
#include "huffman.h"
#include "huffPrefix.h"
#include<string.h>

void intToBinary(int value, int bits, char *out) {
    for (int i = bits - 1; i >= 0; i--) {
        out[bits - 1 - i] = (value & (1 << i)) ? '1' : '0';
    }
    out[bits] = '\0';
}

int32_t binaryToInt(char *string){
    int32_t num = 0;
    int32_t len = strlen(string);
    char temp[len+1];
    strcpy(temp, string);
    if(temp[0]=='0'){
        for (int i = 0; i < len; i++) {
                temp[i] = (temp[i] == '1') ? '0' : '1';
        }
        for(int i = 0; i<len;i++){
            if(temp[i]=='1'){
                num+= 1 << (len - i - 1);
            }
        }
        return -num;
    }
    for(int i = 0; i<len;i++){
        if(string[i]=='1'){
            num+= 1 << (len - i - 1);
        }
    }
    return num;
}


void bitPrint(char *string, int32_t final, FILE *fp, char *buffer, int32_t *buffer_capacity){
    int i=0;
    while(i < strlen(string)){
        *buffer = (*buffer<<1) | (string[i]=='1');
        (*buffer_capacity)++;
        if(*buffer_capacity == 8){
            fputc(*buffer,fp);
            *buffer = 0;
            *buffer_capacity = 0;
        }
        i++;
    }
    if(final){
        (*buffer) <<= (8 - *buffer_capacity);
        fputc(*buffer,fp);
    }
}

int32_t bitReadDC(FILE *fp, char *sbuffer, char *buffer, int32_t *buffer_level){
    int32_t done = 0, num;
    int32_t category = 0;
    while(done==0){
        if(strlen(sbuffer)!=0){
            for(int i = 0;i<11;i++){
                if(strcmp(sbuffer,DCprefix[i])==0){
                    category = i;
                    strcpy(sbuffer,"");
                    for(int j = 0; j<category;j++){
                        if(*buffer_level==0){
                            *buffer = fgetc(fp);
                            (*buffer_level)=8;
                        }
                        if((*buffer & 0b10000000) == 0b10000000){
                            strcat(sbuffer,"1");
                            *buffer<<=1;
                            (*buffer_level)--;
                        }
                        else{
                            strcat(sbuffer,"0");
                            *buffer<<=1;
                            (*buffer_level)--;
                        }
                    }
                    num = binaryToInt(sbuffer);
                    strcpy(sbuffer,"");
                    done = 1;
                    return num;
                    break;
                }
            }
        }
        if(!done){
            if((*buffer_level)==0){
            *buffer = fgetc(fp);
            (*buffer_level)=8;
            }
            if((*buffer & 0b10000000) == 0b10000000){
                    strcat(sbuffer,"1");
                    (*buffer)<<=1;
                    (*buffer_level)--;
            }
            else{
                strcat(sbuffer,"0");
                (*buffer)<<=1;
                (*buffer_level)--;
            }

        }
    }
    return -1;
}


int32_t categorize(int32_t component){
    int32_t category;
        if (component == 0) {
            category = 0;
        }
        else if (component == 1 || component == -1) {
            category = 1;
        }
        else if ((component >= -3 && component <= -2) || (component >= 2 && component <= 3)) {
            category = 2;
        }
        else if ((component >= -7 && component <= -4) || (component >= 4 && component <= 7)) {
            category = 3;
        }
        else if ((component >= -15 && component <= -8) || (component >= 8 && component <= 15)) {
            category = 4;
        }
        else if ((component >= -31 && component <= -16) || (component >= 16 && component <= 31)) {
            category = 5;
        }
        else if ((component >= -63 && component <= -32) || (component >= 32 && component <= 63)) {
            category = 6;
        }
        else if ((component >= -127 && component <= -64) || (component >= 64 && component <= 127)) {
            category = 7;
        }
        else if ((component >= -255 && component <= -128) || (component >= 128 && component <= 255)) {
            category = 8;
        }
        else if ((component >= -511 && component <= -256) || (component >= 256 && component <= 511)) {
            category = 9;
        }
        else if ((component >= -1023 && component <= -512) || (component >= 512 && component <= 1023)) {
            category = 10;
        }
        else if ((component >= -2047 && component <= -1024) || (component >= 1024 && component <= 2047)) {
            category = 11;
        }
        else {
            category = -1;
        }
    return category;
}

void encodeDC(int32_t DC, FILE *fp, char *buffer, int32_t *buffer_capacity){ 
    int category = categorize(DC);
    char mantissa[11];
    char string[24];

    if (category == 0) {
        string[0] = '\0';  
    } else {
        if (DC > 0) {
            intToBinary(DC, category, mantissa);
        } else {
            intToBinary(abs(DC), category, mantissa);
            for (int i = 0; i < category; i++) {
                mantissa[i] = (mantissa[i] == '1') ? '0' : '1';
            }
        }
    }

    strcpy(string,DCprefix[category]);
    strcat(string,mantissa);
    bitPrint(string, 0, fp, buffer, buffer_capacity);
}


void encodeAC(pair AC[63], FILE *fp, char *buffer, int32_t *buffer_capacity, int32_t final){
    char mantissa[18];
    char string[28];
    for(int i = 0; i <63; i++){
        int category = categorize(AC[i].second);
        if (category == 0) {
            string[0] = '\0';  
        } 
        else {
            if (AC[i].second > 0) {
                intToBinary(AC[i].second, category, mantissa);
            } else {
                intToBinary(abs(AC[i].second), category, mantissa);
                for (int i = 0; i < category; i++) {
                    mantissa[i] = (mantissa[i] == '1') ? '0' : '1';
                }
            }
        }
        strcpy(string,ACprefix[(AC[i]).first][category]);
        strcat(string,mantissa);
        if(i==62 || (AC[i].first == 0 && AC[i].second == 0)){
            bitPrint(string, final, fp, buffer, buffer_capacity);
        }
        else{
            bitPrint(string, 0, fp, buffer, buffer_capacity);
        }
    }
}


void encodeDCmatrix(int32_t height,int32_t width, int32_t **DC, FILE *fp, char *buffer, int32_t *buffer_capacity){
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
            encodeDC(DC[i][j],fp, buffer, buffer_capacity);
        }
    }
}

void decodeDCmatrix(int32_t height,int32_t width, int32_t **DC,FILE *fp, char *buffer, char *sbuffer, int32_t *buffer_level){
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
             DC[i][j] = bitReadDC(fp,sbuffer, buffer, buffer_level);
        }
    }
}

void encodeACmatrix(int32_t height,int32_t width, pair (**AC)[63], FILE *fp, char *buffer, int32_t *buffer_capacity, int32_t final){
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
            if(i == (height -1) && j == (width -1)){
                encodeAC(AC[i][j],fp, buffer, buffer_capacity, final);
            }
            else{
                encodeAC(AC[i][j],fp, buffer, buffer_capacity, 0);
            }

        }
    }
}

void huffman_encode(rlediff_img *rlediff_img, char *fname){
    char buffer = 0;
    int32_t buffer_capacity = 0;
    FILE *fp = fopen(fname,"wb");
    int32_t height = rlediff_img->height;
    int32_t width = rlediff_img->width;

    fwrite(&height,sizeof(height),1,fp);
    fwrite(&width,sizeof(width),1,fp);

    printf("%s\n","vai tomando playboy");


    encodeDCmatrix(height,width,rlediff_img->Y_block_DC, fp, &buffer, &buffer_capacity);
    encodeDCmatrix(height/2,width/2,rlediff_img->Cr_block_DC, fp,  &buffer, &buffer_capacity);
    encodeDCmatrix(height/2,width/2,rlediff_img->Cb_block_DC, fp,  &buffer, &buffer_capacity);
    /*
    encodeACmatrix(height,width,rlediff_img->Y_block_AC, fp, &buffer, &buffer_capacity,0);
    encodeACmatrix(height/2,width/2,rlediff_img->Cr_block_AC, fp,  &buffer, &buffer_capacity,0);
    encodeACmatrix(height/2,width/2,rlediff_img->Cb_block_AC, fp,  &buffer, &buffer_capacity,1);
    */
    fclose(fp);
}

rlediff_img *huffman_decode(char *huffman_name){
    char buffer = 0, sbuffer[32] = ""; 
    int32_t height =0, width = 0, buffer_level= 0;
    FILE *fp = fopen(huffman_name,"rb");
    fread(&height,sizeof(height),1,fp);
    fread(&width,sizeof(width),1,fp);

    rlediff_img *rle_img = rlediff_img_alloc(height, width);
    decodeDCmatrix(height,width,rle_img->Y_block_DC,fp,&buffer,sbuffer,&buffer_level);
    //decodeDCmatrix(height/2,width/2,rle_img->Cr_block_DC,fp,&buffer,sbuffer,&buffer_level);
    //decodeDCmatrix(height/2,width/2,rle_img->Cb_block_DC,fp,&buffer,sbuffer,&buffer_level);

    fclose(fp);
    return rle_img;
}

