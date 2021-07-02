#ifndef PROGA_BMP_H
#define PROGA_BMP_H

typedef struct cBITMAPFILEHEADER56{
    unsigned short type;
    unsigned int size;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int offBits;
} BITMAPFILEHEADER56;

typedef struct cBITMAPINFO56{
    unsigned int size;
    long width;
    long height;
    unsigned short planes;
    unsigned short bitCount;
    unsigned int compression;
    unsigned int sizeImage;
    long xPelsPerMeter;
    long yPelsPerMeter;
    unsigned int clrUsed;
    unsigned int clrImportant;
    unsigned int* colorTable;
} BITMAPINFO56;

typedef struct sBITMAP56 {
    BITMAPINFO56 bitmapInfo;
    unsigned int* data;
} BITMAP56;

BITMAP56* readBmp(const char* buffer);
char* writeBmp(BITMAP56* bitmap, int* length);
char* getLastBmpError();
#endif
