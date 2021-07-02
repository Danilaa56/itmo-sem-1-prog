#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int myBmpErrorCode56 = 0;

char* getLastBmpError(){
    switch(myBmpErrorCode56){
        case 0:
            return "OK";
        case 1:
            return "Data signature is neither BM nor MB";
        case 2:
            return "BMP with unsupported BITMAPINFO size";
        case 3:
            return "One of the reserved fields in bitmap header or both are not 0";
        case 4:
            return "Planes count must be 1";
        case 5:
            return "xPelsPerMeter, yPels., compression, clrUsed or clrImportant is not 0";
        case 6:
            return "Bit count is neither 24 nor 1";
        case 7:
            return "Could not allocate memory";
        default:
            return "Unknown error";
    }
}

unsigned int readUint(const char* bytes){
    const unsigned char* uBytes = (unsigned char*) bytes;
    return (uBytes[0]<<24) | (uBytes[1]<<16)| (uBytes[2]<<8) | (uBytes[3]);
}
unsigned short readUshort(const char* bytes){
    const unsigned char* uBytes = (unsigned char*) bytes;
    return (uBytes[0]<<8) | (uBytes[1]);
}

unsigned int readUintBigEndian(const char* bytes){
    const unsigned char* uBytes = (unsigned char*) bytes;
    return (uBytes[3]<<24) | (uBytes[2]<<16)| (uBytes[1]<<8) | (uBytes[0]);
}
unsigned short readUshortBigEndian(const char* bytes){
    const unsigned char* uBytes = (unsigned char*) bytes;
    return (uBytes[1]<<8) | (uBytes[0]);
}

void writeUshort(char* bytes, unsigned short value){
    bytes[0] = (value>>8)&0xff;
    bytes[1] = (value)&0xff;
}

void writeUint(char* bytes, unsigned int value){
    bytes[0] = (value>>24)&0xff;
    bytes[1] = (value>>16)&0xff;
    bytes[2] = (value>>8)&0xff;
    bytes[3] = (value)&0xff;
}

void writeUshortBigEndian(char* bytes, unsigned short value){
    bytes[1] = (value>>8)&0xff;
    bytes[0] = (value)&0xff;
}

void writeUintBigEndian(char* bytes, unsigned int value){
    bytes[3] = (value>>24)&0xff;
    bytes[2] = (value>>16)&0xff;
    bytes[1] = (value>>8)&0xff;
    bytes[0] = (value)&0xff;
}

BITMAP56* readBmp(const char* buffer){
    typedef unsigned int uint;
    typedef unsigned short ushort;

    BITMAPFILEHEADER56 header;

    char isBigEndian;

    header.type = readUshort(buffer);
    if(header.type == 0x424d){
        isBigEndian = 1;
    } else if(header.type == 0x4d42){
        isBigEndian = 0;
    } else {
        printf("Wrong header type: %c%c\n", (char)(header.type>>8), (char)(header.type));
        myBmpErrorCode56 = 1;
        return 0;
    }

    BITMAPINFO56 bitmapInfo;

    if(isBigEndian){
        header.size = readUintBigEndian(buffer+2);
        header.reserved1 = readUshortBigEndian(buffer+6);
        header.reserved2 = readUshortBigEndian(buffer+8);
        header.offBits = readUintBigEndian(buffer+10);

        bitmapInfo.size = readUintBigEndian(buffer + 0x0e);
        if(bitmapInfo.size == 12){
            bitmapInfo.width = readUshortBigEndian(buffer + 0x12);
            bitmapInfo.height = readUshortBigEndian(buffer + 0x14);
            bitmapInfo.planes = readUshortBigEndian(buffer + 0x16);
            bitmapInfo.bitCount = readUshortBigEndian(buffer + 0x18);
        } else if(bitmapInfo.size == 40){
            bitmapInfo.width = (long)readUintBigEndian(buffer + 0x12);
            bitmapInfo.height = (long)readUintBigEndian(buffer + 0x16);
            bitmapInfo.planes = readUshortBigEndian(buffer + 0x1a);
            bitmapInfo.bitCount = readUshortBigEndian(buffer + 0x1c);

            bitmapInfo.compression = readUshortBigEndian(buffer + 0x1e);
            bitmapInfo.sizeImage = readUintBigEndian(buffer + 0x22);
            bitmapInfo.xPelsPerMeter = (long)readUintBigEndian(buffer + 0x26);
            bitmapInfo.yPelsPerMeter = (long)readUintBigEndian(buffer + 0x2a);
            bitmapInfo.clrUsed = readUshortBigEndian(buffer + 0x2e);
            bitmapInfo.clrImportant = readUshortBigEndian(buffer + 0x32);
        } else{
            printf("BMP with such BITMAPINFO size: %d-  is unsupported\n", bitmapInfo.size);
            myBmpErrorCode56 = 2;
            return 0;
        }
    } else {
        header.size = readUint(buffer+2);
        header.reserved1 = readUshort(buffer+6);
        header.reserved2 = readUshort(buffer+8);
        header.offBits = readUint(buffer+10);

        bitmapInfo.size = readUint(buffer + 0x0e);
        if(bitmapInfo.size == 12){
            bitmapInfo.width = readUshort(buffer + 0x12);
            bitmapInfo.height = readUshort(buffer + 0x14);
            bitmapInfo.planes = readUshort(buffer + 0x16);
            bitmapInfo.bitCount = readUshort(buffer + 0x18);
        } else if(bitmapInfo.size == 40){
            bitmapInfo.width = (long)readUint(buffer + 0x12);
            bitmapInfo.height = (long)readUint(buffer + 0x16);
            bitmapInfo.planes = readUshort(buffer + 0x1a);
            bitmapInfo.bitCount = readUshort(buffer + 0x1c);

            bitmapInfo.compression = readUint(buffer + 0x1e);
            bitmapInfo.sizeImage = readUint(buffer + 0x22);
            bitmapInfo.xPelsPerMeter = (long)readUint(buffer + 0x26);
            bitmapInfo.yPelsPerMeter = (long)readUint(buffer + 0x2a);
            bitmapInfo.clrUsed = readUint(buffer + 0x2e);
            bitmapInfo.clrImportant = readUint(buffer + 0x32);
        } else{
            printf("BMP with such BITMAPINFO size: %d-  is unsupported\n", bitmapInfo.size);
            myBmpErrorCode56 = 2;
            return 0;
        }
    }

    /*printf("Header file size: %u\n", header.size);
    printf("Header reserved 1: %u\n", header.reserved1);
    printf("Header reserved 2: %u\n", header.reserved2);
    printf("Header offset bytes: %u\n\n", header.offBits);

    printf("Bitmap info size: %u\n", bitmapInfo.size);
    printf("Bitmap info width: %ld\n", bitmapInfo.width);
    printf("Bitmap info height: %ld\n", bitmapInfo.height);
    printf("Bitmap info planes: %u\n", bitmapInfo.planes);
    printf("Bitmap info bit count: %u\n", bitmapInfo.bitCount);

    if(bitmapInfo.size == 40){
        printf("Bitmap info compression: %u\n", bitmapInfo.compression);
        printf("Bitmap info sizeImage: %u\n", bitmapInfo.sizeImage);
        printf("Bitmap info xPelsPerMeter: %ld\n", bitmapInfo.xPelsPerMeter);
        printf("Bitmap info yPelsPerMeter: %ld\n", bitmapInfo.yPelsPerMeter);
        printf("Bitmap info colors used: %u\n", bitmapInfo.clrUsed);
        printf("Bitmap info colors important: %u\n", bitmapInfo.clrImportant);
    }*/

    if(header.reserved1 != 0 || header.reserved2 != 0){
        myBmpErrorCode56 = 3;
        return 0;
    }

    if(bitmapInfo.planes != 1){
        myBmpErrorCode56 = 4;
        return 0;
    }

    if(bitmapInfo.xPelsPerMeter || bitmapInfo.yPelsPerMeter || bitmapInfo.clrImportant || bitmapInfo.clrUsed || bitmapInfo.compression){
        myBmpErrorCode56 = 5;
        return 0;
    }

    if(bitmapInfo.bitCount != 24 && bitmapInfo.bitCount != 1){
        myBmpErrorCode56 = 6;
        return 0;
    }

    uint* data = malloc(bitmapInfo.height*bitmapInfo.width*4);
    if(!data){
        myBmpErrorCode56 = 7;
        return 0;
    }

    buffer += bitmapInfo.size+14;
    if(bitmapInfo.bitCount == 1){
        uint* colors = malloc(4*2);
        if(!colors){
            free(data);
            myBmpErrorCode56 = 7;
            return 0;
        }
        if(isBigEndian){
            colors[0] = readUintBigEndian(buffer);
            colors[1] = readUintBigEndian(buffer+4);
        }else{
            colors[0] = readUint(buffer);
            colors[1] = readUint(buffer+4);
        }
        bitmapInfo.colorTable = colors;
        buffer += 8;

        for(int y=0; y<bitmapInfo.height; y++){
            for(int x=0; x<bitmapInfo.width; x++){
                data[y*bitmapInfo.width + x] = (buffer[(x>>3)]>>(7-(x&7)))&1;
            }
            buffer += (((bitmapInfo.width+7)/8+3)/4)*4;
        }
    } else if(bitmapInfo.bitCount == 24){
        for(int y=0; y<bitmapInfo.height; y++){
            const unsigned char* uBuffer = (unsigned char*) buffer;
            for(int x=0; x<bitmapInfo.width; x++){
                data[y*bitmapInfo.height + x] = (uBuffer[0]<<16) | (uBuffer[1]<<8) | (uBuffer[2]);
                uBuffer += 3;
            }
            buffer += ((bitmapInfo.width*3+3)/4)*4;
        }
    }

    BITMAP56* bitmap = malloc(sizeof(BITMAP56));
    if(!bitmap){
        free(data);
        free(bitmapInfo.colorTable);
        myBmpErrorCode56 = 7;
        return 0;
    }
    bitmap->bitmapInfo = bitmapInfo;
    bitmap->data = data;

    return bitmap;
}

char* writeBmp(BITMAP56* bitmap, int* length){
    if(!bitmap)
        return NULL;

    BITMAPINFO56* bitmapInfo = &bitmap->bitmapInfo;

    if(bitmapInfo->size==12){
        bitmapInfo->size = 40;

        bitmapInfo->compression = 0;
        if(bitmapInfo->bitCount==1){
            bitmapInfo->sizeImage = bitmapInfo->height*((bitmapInfo->width+31)/32)*4;
        } else if(bitmapInfo->bitCount == 24){
            bitmapInfo->sizeImage = bitmapInfo->height*((bitmapInfo->width+3)/4)*4;
        } else {
            myBmpErrorCode56 = 6;
            return 0;
        }
        bitmapInfo->xPelsPerMeter = 0;
        bitmapInfo->yPelsPerMeter = 0;
        bitmapInfo->clrUsed = 0;
        bitmapInfo->clrImportant = 0;
    }

    BITMAPFILEHEADER56 header;
    header.type = 0x4d42;
    header.offBits = bitmapInfo->bitCount==1 ? 62 : 54;
    header.reserved1 = 0;
    header.reserved2 = 0;
    header.size = bitmapInfo->sizeImage + header.offBits;
    char* bufferToReturn = malloc(header.size);
    char* buffer = bufferToReturn;
    if(!buffer){
        myBmpErrorCode56 = 7;
        return 0;
    }

    writeUshortBigEndian(buffer, header.type);
    writeUintBigEndian(buffer+0x02, header.size);
    writeUshortBigEndian(buffer+0x06, header.reserved1);
    writeUshortBigEndian(buffer+0x08, header.reserved2);
    writeUintBigEndian(buffer+0x0a, header.offBits);

    writeUintBigEndian(buffer+0x0e, bitmapInfo->size);
    writeUintBigEndian(buffer+0x12, bitmapInfo->width);
    writeUintBigEndian(buffer+0x16, bitmapInfo->height);
    writeUshortBigEndian(buffer+0x1a, bitmapInfo->planes);
    writeUshortBigEndian(buffer+0x1c, bitmapInfo->bitCount);

    writeUintBigEndian(buffer+0x1e, bitmapInfo->compression);
    writeUintBigEndian(buffer+0x22, bitmapInfo->sizeImage);
    writeUintBigEndian(buffer+0x26, bitmapInfo->xPelsPerMeter);
    writeUintBigEndian(buffer+0x2a, bitmapInfo->yPelsPerMeter);
    writeUintBigEndian(buffer+0x2e, bitmapInfo->clrUsed);
    writeUintBigEndian(buffer+0x32, bitmapInfo->clrImportant);

    if(bitmapInfo->bitCount == 1){
        writeUintBigEndian(buffer+54, bitmapInfo->colorTable[0]);
        writeUintBigEndian(buffer+58, bitmapInfo->colorTable[1]);
        buffer += 62;
    } else {
        buffer += 54;
    }
    if(bitmapInfo->bitCount == 1){
        for(int y=0; y<bitmapInfo->height; y++){
            for(int x=0; x<bitmapInfo->width; x++){
                if((x&7) == 0){
                    buffer[x/8] = 0;
                }
                buffer[x/8] = buffer[x/8] | (bitmap->data[y*bitmapInfo->width + x] ? 1 : 0)<<(7-(x&7));
            }
            int usedBytes = (bitmapInfo->width+7)/8;
            for(int x=((usedBytes+3)/4)*4-1;x>=usedBytes;x--){
                buffer[x] = 0;
            }
            buffer += ((usedBytes+3)/4)*4;
        }
    } else if(bitmapInfo->bitCount == 24){
        for(int y=0; y<bitmapInfo->height; y++){
            unsigned char* uBuffer = (unsigned char*) buffer;
            for(int x=0; x<bitmapInfo->width; x++){
                int pixelColor = bitmap->data[y*bitmapInfo->height+x];
                uBuffer[0] = (pixelColor>>16)&0xff;
                uBuffer[1] = (pixelColor>>8)&0xff;
                uBuffer[2] = (pixelColor)&0xff;
                uBuffer += 3;
            }
            int usedBytes = bitmapInfo->width*3;
            for(int x=((usedBytes+3)/4)*4-1;x>=usedBytes;x--){
                buffer[x] = 0;
            }
            buffer += ((usedBytes+3)/4)*4;
        }
    }
    *length = header.size;
    return bufferToReturn;
}