#include<stdio.h>
#include<math.h>
#include "huffman.h"
#include "huffPrefix.h"
#include<string.h>


//converts an int into a string with its binary equivalent
void intToBinary(int value, int bits, char *out) {
        for (int i = bits - 1; i >= 0; i--) {
            out[bits - 1 - i] = (value & (1 << i)) ? '1' : '0';
        }
        out[bits] = '\0';
}


//converts a string that contains a binary number into its integer equivalent
int32_t binaryToInt(char *string) {
    int32_t num = 0;
    int32_t len = strlen(string);

    if (len == 0) return 0;

    if (string[0] == '1') {
        for (int i = 0; i < len; i++) {
            if (string[i] == '1') {
                num += 1 << (len - i - 1);
            }
        }
        return num;
    } else {
      
        for (int i = 0; i < len; i++) {
            if (string[i] == '0') {
                num += 1 << (len - i - 1);
            }
        }
        return -num;
    }
}

//given a string of binary prints it char by char in a file, stores the bits in a buffer until it makes a full char
void bitPrint(char *string, int32_t final, FILE *fp, char *buffer, int32_t *buffer_capacity){
    int32_t i=0;
    while(i < (int32_t) strlen(string)){
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
        //if these are the last bits of an encoding but the buffer isnt full, fills it with zeroes 
        (*buffer) <<= (8 - *buffer_capacity);
        fputc(*buffer,fp);
    }
}

//reads a DC component from the file
int32_t bitReadDC(FILE *fp, char *sbuffer, char *buffer, int32_t *buffer_level){
    int32_t done = 0, num;
    int32_t category = 0;
    while(done==0){
        if(strlen(sbuffer)!=0){
            for(int i = 0;i<11;i++){
                //if the buffer isnt empty checks if it mathes a prefix
                if(strcmp(sbuffer,DCprefix[i])==0){
                    //category determines how many more bits to read
                    category = i;
                    strcpy(sbuffer,"");
                    for(int j = 0; j<category;j++){
                        if(*buffer_level==0){
                            *buffer = fgetc(fp);
                            (*buffer_level)=8;
                        }
                        if(((*buffer) & 0b10000000) == 0b10000000){
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
                    //after reading all the bits converts it to integer
                    num = binaryToInt(sbuffer);
                    strcpy(sbuffer,"");
                    done = 1;
                    return num;
                    break;
                }
            }
        }
        if(!done){
            //reads more chars from the file if needed
            if((*buffer_level)==0){
            (*buffer) = fgetc(fp);
            (*buffer_level)=8;
            }
            if(((*buffer) & 0b10000000) == 0b10000000){
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

//reads an AC component from the file and return it as a pair
pair *bitReadAC(FILE *fp, char *sbuffer, char *buffer, int32_t *buffer_level) {
    int32_t done = 0;
    int32_t category = 0;
    pair *result = malloc(sizeof(pair));

    while (done == 0) {
        if (strlen(sbuffer) != 0) {
            //if the buffer is not empty compares it with all the prefix
            for (int i = 0; i < 16 && !done; i++) {
                for (int j = 0; j < 11 && !done; j++) {
                    // Check for non-empty prefix to avoid matching "" with ""
                    if (ACprefix[i][j][0] != '\0' && strcmp(sbuffer, ACprefix[i][j]) == 0) {
                        strcpy(sbuffer, "");
                         //the category determines how many bits to read
                        category = j;
                        //the rest of the prefix determines how many zeroes were inbetween the zeroes
                        result->first = i;

                        if (i == 0 && j == 0) { // EOB token
                            result->second = 0;
                            done = 1;
                            break; // Break from j-loop
                        }

                        //reads the mantissa bits
                        for (int k = 0; k < category; k++) {
                            if (*buffer_level == 0) {
                                *buffer = fgetc(fp);
                                *buffer_level = 8;
                            }
                            if (((*buffer) & 0b10000000) == 0b10000000) {
                                strcat(sbuffer, "1");
                            } else {
                                strcat(sbuffer, "0");
                            }
                            (*buffer) <<= 1;
                            (*buffer_level)--;
                        }
                        //converts the mantissa bits to integer
                        result->second = binaryToInt(sbuffer);
                        strcpy(sbuffer, "");
                        done = 1;
                        break; 
                    }
                }
            }
        }

        if (!done) {
            //reads more chars from the file if needed
            if (*buffer_level == 0) {
                *buffer = fgetc(fp);
                *buffer_level = 8;
            }
            if (((*buffer) & 0b10000000) == 0b10000000) {
                strcat(sbuffer, "1");
            } else {
                strcat(sbuffer, "0");
            }
            (*buffer) <<= 1;
            (*buffer_level)--;
        }
    }
    return result;
}

//determines the category of a given integer
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

//given an DC component as an integer, huffman encode it and print it to a file
void encodeDC(int32_t DC, FILE *fp, char *buffer, int32_t *buffer_capacity){ 
    //categorizes the DC component
    int category = categorize(DC);
    char mantissa[11] = "";
    char string[24];

    if (category == 0) {
        string[0] = '\0';  
    } else {
        if (DC >=0) {
            //converts it to binary
            intToBinary(DC, category, mantissa);
        }
        else if(DC==0){

        }
        else {
            //inverts the number if its negatives for huffman encoding and converts it to binary
            intToBinary(abs(DC), category, mantissa);
            for (int i = 0; i < category; i++) {
                mantissa[i] = (mantissa[i] == '1') ? '0' : '1';
            }
        }
    }

    //joins the category and the mantissa and prints it to the file
    strcpy(string,DCprefix[category]);
    strcat(string,mantissa);
    bitPrint(string, 0, fp, buffer, buffer_capacity);
}

//given the AC components as an array of pairs of ints, huffman encodes and prints them to the file
void encodeAC(pair AC[63], FILE *fp, char *buffer, int32_t *buffer_capacity, int32_t final){
    char mantissa[18];
    char string[30];
    for(int i = 0; i <63; i++){
          //categorizes the AC component
        int category = categorize(AC[i].second);
        if (category == 0) {
            //if the category is 0 the mantissa is empty
            strcpy(mantissa,"");
        } 
        else{
            if (AC[i].second > 0) {
                //converts the mantissa to binary
                intToBinary(AC[i].second, category, mantissa);
            } else {
                //inverts the negative number and converts it to binary
                intToBinary(abs(AC[i].second), category, mantissa);
                for (int j = 0; j < category; j++) {
                    mantissa[j] = (mantissa[j] == '1') ? '0' : '1';
                }
            }
        }
        //joins the prefix and the mantissa and prints it to the file
        strcpy(string,ACprefix[(AC[i]).first][category]);
        strcat(string,mantissa);
        if(i==62 || (AC[i].first == 0 && AC[i].second == 0)){
            //if its the last meaningful pair of the array, print and break;
            bitPrint(string, final, fp, buffer, buffer_capacity);
            break;
        }
        else{
            bitPrint(string, 0, fp, buffer, buffer_capacity);
        }
    };
    
}


//encodes the DC matrix into the file
void encodeDCmatrix(int32_t height,int32_t width, int32_t **DC, FILE *fp, char *buffer, int32_t *buffer_capacity){
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
            encodeDC(DC[i][j],fp, buffer, buffer_capacity);
        }
    }
}


//decode the DC matrix from the file into a matrix of ints
void decodeDCmatrix(int32_t height,int32_t width, int32_t **DC,FILE *fp, char *buffer, char *sbuffer, int32_t *buffer_level){
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width; j++){
             DC[i][j] = bitReadDC(fp,sbuffer, buffer, buffer_level);
        }
    }
}

//encodes the AC matrix into the file
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

//decode the AC matrix from the file into a matrix of arrays of pair of ints
void decodeACmatrix(int32_t height,int32_t width, pair (**AC)[63], FILE *fp, char *buffer, char *sbuffer, int32_t *buffer_level){
    for(int i = 0; i < height;i++){
        for(int j = 0; j < width;j++){
             pair *result;
             int x, y;
            for(int k = 0; k < 63; k++){
                result = bitReadAC(fp,sbuffer,buffer, buffer_level);
                x = result->first;
                y = result->second;
                AC[i][j][k].first = result->first;
                AC[i][j][k].second = result->second;
                free(result);
                if(x == 0 && y == 0){
                    break;
                }
                
            }
        }
    }

}

//huffman encodes a struct that contains the AC components runlenght enconded and the DC components difference encoded into a file
void huffman_encode(rlediff_img *rlediff_img, char *fname){
    char buffer = 0;
    int32_t buffer_capacity = 0;
    FILE *fp = fopen(fname,"wb");
    int32_t height = rlediff_img->height;
    int32_t width = rlediff_img->width;
    float compression_factor = rlediff_img->compression_factor;

    //writes the "header"
    fwrite(&height,sizeof(height),1,fp);
    fwrite(&width,sizeof(width),1,fp);
    fwrite(&compression_factor,sizeof(compression_factor),1,fp);

    //encodes the the DC components
    encodeDCmatrix(height,width,rlediff_img->Y_block_DC, fp, &buffer, &buffer_capacity);
    encodeDCmatrix(height/2,width/2,rlediff_img->Cr_block_DC, fp,  &buffer, &buffer_capacity);
    encodeDCmatrix(height/2,width/2,rlediff_img->Cb_block_DC, fp,  &buffer, &buffer_capacity);


    //encodes the AC components
    encodeACmatrix(height,width,rlediff_img->Y_block_AC, fp, &buffer, &buffer_capacity,0);
    encodeACmatrix(height/2,width/2,rlediff_img->Cr_block_AC, fp,  &buffer, &buffer_capacity,0);
    encodeACmatrix(height/2,width/2,rlediff_img->Cb_block_AC, fp,  &buffer, &buffer_capacity,1);
    
    fclose(fp);
}

//decodes a file using huffman into a struct that contains the AC components runlenght enconded and the DC components difference encoded
rlediff_img *huffman_decode(char *huffman_name){
    char buffer = 0, sbuffer[48] = ""; 
    int32_t height =0, width = 0, buffer_level= 0;
    float compression_factor = 0;
    FILE *fp = fopen(huffman_name,"rb");

    //reads the "header"
    fread(&height,sizeof(height),1,fp);
    fread(&width,sizeof(width),1,fp);
    fread(&compression_factor,sizeof(compression_factor),1,fp);


    //allocates the struct
    rlediff_img *rle_img = rlediff_img_alloc(height, width,compression_factor);

    //decodes the DC components
    decodeDCmatrix(height,width,rle_img->Y_block_DC,fp,&buffer,sbuffer,&buffer_level);
    decodeDCmatrix(height/2,width/2,rle_img->Cr_block_DC,fp,&buffer,sbuffer,&buffer_level);
    decodeDCmatrix(height/2,width/2,rle_img->Cb_block_DC,fp,&buffer,sbuffer,&buffer_level);


    //decodes the AC components
    decodeACmatrix(height,width,rle_img->Y_block_AC,fp,&buffer,sbuffer,&buffer_level);
    decodeACmatrix(height/2,width/2,rle_img->Cr_block_AC,fp,&buffer,sbuffer,&buffer_level);
    decodeACmatrix(height/2,width/2,rle_img->Cb_block_AC,fp,&buffer,sbuffer,&buffer_level);

    fclose(fp);
    return rle_img;
}

