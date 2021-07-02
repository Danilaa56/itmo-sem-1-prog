#include <stdio.h>
#include <stdlib.h>

char* readFile(const char* filePath, int* length){
    FILE* file = fopen(filePath, "rb");
    if(!file)
        return NULL;
    fseek(file, 0, SEEK_END);
    *length = (int)ftell(file);
    char* tmpBuffer = malloc(*length);
    if(!tmpBuffer)
        return NULL;
    rewind(file);
    fread(tmpBuffer, *length, 1, file);
    fclose(file);
    return tmpBuffer;
}

int writeFile(char* filePath, int length, void* buffer){
    FILE* file = fopen(filePath, "wb");
    if(!file)
        return 1;
    int tmpValue = fwrite(buffer, 1, length, file);
    fclose(file);
    return tmpValue != length;
}