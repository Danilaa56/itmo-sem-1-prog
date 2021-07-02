#include <stdlib.h>
#include <stdio.h>

#include "lib56/files.h"
#include "lib56/parseInt.h"
#include "lib56/stringsCompare.h"
#include "lib56/file/bmp.h"

int countNeighboursLifeOnBmp(int x, int y, int width, int height, const unsigned int* data){
    #define getPixel(x, y) !data[(y)*width + x]
    int count = 0;
    if(0 < x){
        if(getPixel(x-1,y))
            count++;
        if(0 < y){
            if(getPixel(x-1,y-1))
                count++;
        }
        if(y+1 < height){
            if(getPixel(x-1,y+1))
                count++;
        }
    }
    if(x+1 < width){
        if(getPixel(x+1,y))
            count++;
        if(0 < y){
            if(getPixel(x+1,y-1))
                count++;
        }
        if(y+1 < height){
            if(getPixel(x+1,y+1))
                count++;
        }
    }
    if(0 < y){
        if(getPixel(x,y-1))
            count++;
    }
    if(y+1 < height){
        if(getPixel(x,y+1)){
            count++;
        }
    }
    return count;
}

void iterLifeOnBmp(BITMAP56* bitmap, unsigned int** dataBuffer){
    int worldWidth = bitmap->bitmapInfo.width;
    int worldHeight = bitmap->bitmapInfo.height;

    for(int y=0; y<worldHeight; y++){
        for(int x=0; x<worldWidth; x++){
            int count = countNeighboursLifeOnBmp(x, y, worldWidth, worldHeight, bitmap->data);
            //printf("%d ", count);
            int index = y*worldWidth+x;
            if(bitmap->data[index]){
                dataBuffer[0][index] = (count==3) ? 0 : 1;
            } else {
                dataBuffer[0][index] = (count<2 || count>3) ? 1 : 0;
            }
        }
        //printf("\n");
    }
    unsigned int* tmp_pointer = bitmap->data;
    bitmap->data = dataBuffer[0];
    dataBuffer[0] = tmp_pointer;
}

int stringCopy(char* dest, const char* src){
    int length = 0;
    while(1){
        dest[length] = src[length];
        if(!src[length])
            return length;
        length++;
    }
}

int main(int argc, char** args){
    char* input = NULL;
    char* output = NULL;
    int max_iter = 10;
    int dump_freq = 1;

    for(int i=1; i<argc; i++){
        if(i+1 == argc)
            break;
        char* nextString = args[i+1];
        if(!stringsCompare(args[i],"--input",7))
            input = nextString;
        else if(!stringsCompare(args[i],"--output",6))
            output = nextString;
        else if(!stringsCompare(args[i],"--max_iter",5)){
            max_iter = parseInt(nextString);
            if(max_iter<0)
                max_iter = 10;
        }
        else if(!stringsCompare(args[i],"--dump_freq",7)){
            dump_freq = parseInt(nextString);
            if(dump_freq<0)
                dump_freq = 1;
        }
    }

    if(!input){
        printf("Input file was not defined\n");
        exit(1);
    }

    int length;
    char* buffer = readFile(input, &length);
    if(!buffer){
        printf("Failed to read file: %s\n", input);
        exit(1);
    }

    BITMAP56* bitmap = readBmp(buffer);
    if(!bitmap){
        printf("Failed to read bmp file: %s\n", input);
        printf("Error: %s\n", getLastBmpError());
        exit(1);
    }

    char outputFile[60];
    int outputFolderLength = stringCopy(outputFile, output);
    outputFile[outputFolderLength] = '\\';
    outputFile[outputFolderLength+4] = '.';
    outputFile[outputFolderLength+5] = 'b';
    outputFile[outputFolderLength+6] = 'm';
    outputFile[outputFolderLength+7] = 'p';
    outputFile[outputFolderLength+8] = 0;

    unsigned int* dataBuffer = malloc(bitmap->bitmapInfo.width * bitmap->bitmapInfo.height * 4);

    for(int i=1; i<=max_iter; i++){
        iterLifeOnBmp(bitmap, &dataBuffer);
        if(!(i%dump_freq)){
            char* outputBmp = writeBmp(bitmap, &length);
            outputFile[outputFolderLength+1] = ((i/100)%10) + '0';
            outputFile[outputFolderLength+2] = ((i/10)%10) + '0';
            outputFile[outputFolderLength+3] = (i%10) + '0';
            int error = writeFile(outputFile, length, outputBmp);
            if(error){
                printf("Couldn't write file: %s\n", outputFile);
            }
            free(outputBmp);
        }
    }
    free(bitmap->data);
    free(bitmap->bitmapInfo.colorTable);
    free(bitmap);
}