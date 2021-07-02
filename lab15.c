#include <stdio.h>
#include <stdlib.h>

#include "lib56/files.h"
#include "lib56/stringsCompare.h"

#define getBit(ar, n) ( (ar[n>>3]>>(7-(n&7)))&1)
#define bitsToBytes(n) ((n+7)/8)
char next8charsAsByte(const char* ch){
    return (char)((ch[0]-'0')<<7) | ((ch[1]-'0')<<6) | ((ch[2]-'0')<<5) | ((ch[3]-'0')<<4) | ((ch[4]-'0')<<3) | ((ch[5]-'0')<<2) | ((ch[6]-'0')<<1) | ((ch[7]-'0'));
}

void stringCopy(char* dest, const char* src, int length){
    int i = 0;
    while (i!=length){
        dest[i] = src[i];
        if(src[i++]==0)
            break;
    }
}
int strLength(const char* str){
    int i = 0;
    while(str[i] != 0)
        i++;
    return i;
}

struct Code{
    unsigned char value;
    int frequency;
};
struct CharBuffer{
    int length;
    char* value;
};
struct CodeLink{
    int frequency;
    int linksCount;
    int* linked;
};
char** makeHuffmanCodes(struct Code codes[], const int uniqueCodesCount){
    struct CharBuffer huffCodes[uniqueCodesCount];
    for(int i=0;i<uniqueCodesCount;i++){
        huffCodes[i].length = 0;
        huffCodes[i].value = malloc(sizeof(char)*uniqueCodesCount);
    }

    struct CodeLink links[uniqueCodesCount];
    for(int i=0;i<uniqueCodesCount;i++){
        links[i].frequency = codes[i].frequency;
        links[i].linksCount = 1;
        links[i].linked = malloc(sizeof(int));
        links[i].linked[0] = i;
    }

    int left = uniqueCodesCount;
    while(left>1){
        int min = links[0].frequency < links[1].frequency ? 0 : 1;
        int min2 = 1-min;
        for(int i=2; i<left; i++){
            if(links[i].frequency < links[min].frequency){
                min2 = min;
                min = i;
            } else if(links[i].frequency < links[min2].frequency){
                min2 = i;
            }
        }

        int* tmp = malloc(sizeof(int)*(links[min].linksCount + links[min2].linksCount));

        for(int i=0; i < links[min].linksCount; i++){
            int childLinkIndex = links[min].linked[i];
            huffCodes[childLinkIndex].value[huffCodes[childLinkIndex].length++] = '0';
            tmp[i] = childLinkIndex;
        }

        for(int i=0; i < links[min2].linksCount; i++){
            int childLinkIndex = links[min2].linked[i];
            huffCodes[childLinkIndex].value[huffCodes[childLinkIndex].length++] = '1';
            tmp[links[min].linksCount+i] = childLinkIndex;
        }

        links[min].frequency += links[min2].frequency;
        links[min].linksCount += links[min2].linksCount;
        free(links[min].linked);
        free(links[min2].linked);
        links[min].linked = tmp;
        links[min2] = links[--left];
    }
    free(links[0].linked);

    char** resultCodes = malloc(sizeof(char*)*uniqueCodesCount);
    for(int i=0; i<uniqueCodesCount; i++){
        int codeLength = huffCodes[i].length;
        resultCodes[i] = malloc(sizeof(char)*(codeLength+1));
        resultCodes[i][codeLength] = 0;
        for(int j=0;j<codeLength;j++)
            resultCodes[i][j] = huffCodes[i].value[codeLength-1-j];
        free(huffCodes[i].value);
    }
    return resultCodes;
}

void writeShort(char* buffer, short value){
    buffer[0] = (char)((value>>8)&0xff);
    buffer[1] = (char)((value)&0xff);
}
void writeInt(char* buffer, int value){
    buffer[0] = (char)((value>>24)&0xff);
    buffer[1] = (char)((value>>16)&0xff);
    buffer[2] = (char)((value>>8)&0xff);
    buffer[3] = (char)((value)&0xff);
}
short readShort(const char* buffer){
    return (short)( ((unsigned char )buffer[0]<<8)|(unsigned char )(buffer[1]) );
}
int readInt(const char* buffer){
    return (int)( ((unsigned char )buffer[0]<<24)|((unsigned char )buffer[1]<<16)|((unsigned char )buffer[2]<<8)|((unsigned char )buffer[3]) );
}

char* writeDictBuffer(int* dictBufferLength, int uniqueCodesCount, struct Code* codes, char** huffCodes){
    char* dictBuffer = malloc(8192);
    char* dictBufferPointer = dictBuffer;

    writeShort(dictBufferPointer, (short)uniqueCodesCount);
    dictBufferPointer += 2;
    for(int i=0; i<uniqueCodesCount; i++){
        *dictBufferPointer++ = codes[i].value;
        *dictBufferPointer++ = (char)strLength(huffCodes[i]);
    }

    char* codesBuffer = malloc(8192);
    char* codesBufferPointer = codesBuffer;
    for(int i=0; i<uniqueCodesCount; i++){
        int j = 0;
        while(huffCodes[i][j]){
            *codesBufferPointer++ = huffCodes[i][j];
            j++;
        }
    }
    while((codesBufferPointer-codesBuffer)&7){
        *codesBufferPointer++ = '0';
    }

    int codesBitsBufferSize = bitsToBytes(codesBufferPointer-codesBuffer);
    for(int i=0; i<codesBitsBufferSize; i++){
        *dictBufferPointer++ = next8charsAsByte(codesBuffer+i*8);
    }
    free(codesBuffer);
    *dictBufferLength = (int)(dictBufferPointer-dictBuffer);
    return dictBuffer;
}
int* translateDataIntoCodes(const unsigned char* data, int length, struct Code* codes, int uniqueCodesCount){
    int* dataByteToIndex = malloc(sizeof(int)*256);
    for(int i=0; i<256; i++){
        for(int j=0; j<uniqueCodesCount; j++){
            if( i == codes[j].value){
                dataByteToIndex[i] = j;
                break;
            }
        }
    }

    int* dataCodes = malloc(sizeof(int)*length);
    for(int i=0; i<length; i++){
        dataCodes[i] = dataByteToIndex[data[i]];
    }
    free(dataByteToIndex);
    return dataCodes;
}
char* compress(const char* data, int length, int* compressedLength){
    const unsigned char* uData = (const unsigned char*)data;

    struct Code codes[256];
    for(int i=0;i<256;i++)
        codes[i].frequency = 0;

    for(int i=0; i<length; i++){
        int index = uData[i];
        codes[index].frequency++;
    }

    int uniqueCodesCount = 0;
    for(unsigned i=0; i<256; i++){
        if(codes[i].frequency != 0){
            codes[uniqueCodesCount].frequency = codes[i].frequency;
            codes[uniqueCodesCount].value = i;
            uniqueCodesCount++;
        }
    }

    int* dataCodes = translateDataIntoCodes(uData, length, codes, uniqueCodesCount);

    char** huffCodes = makeHuffmanCodes(codes, uniqueCodesCount);

    int dictLength;
    char* dictBuffer = writeDictBuffer(&dictLength, uniqueCodesCount, codes, huffCodes);

    char* dataBuffer = malloc(length*8);
    char* dataBufferPointer = dataBuffer;
    for(int i=0; i<length; i++){
        int j = 0;
        while(huffCodes[dataCodes[i]][j]){
            *dataBufferPointer++ = huffCodes[dataCodes[i]][j];
            j++;
        }
    }
    while((dataBufferPointer-dataBuffer)&7){
        *dataBufferPointer++ = '0';
    }
    int dataBitsBufferSize = bitsToBytes(dataBufferPointer-dataBuffer);

    int outputLength = dataBitsBufferSize + dictLength + 4;
    char* outputBuffer = malloc(outputLength);
    char* pointer = outputBuffer;
    writeInt(pointer, length);
    pointer += 4;

    for(int i=0; i<dictLength; i++){
        *pointer++ = dictBuffer[i];
    }
    free(dictBuffer);

    for(int i=0; i<dataBitsBufferSize; i++){
        char c = next8charsAsByte(dataBuffer + i*8);
        *pointer++ = c;
    }

    free(dataBuffer);
    free(dataCodes);

    *compressedLength = outputLength;
    return outputBuffer;
}

struct DecompressCode{
    struct DecompressCode* next[2];
    char isLast;
    char value;
};

struct DecompressCode* readDict(const char* buffer, int* dictLength, int* uniqueCodesCount){
    const char* pointer = buffer;
    int codesCount = readShort(pointer);
    *uniqueCodesCount = codesCount;
    pointer += 2;

    char* codeValues = malloc(sizeof(char) * codesCount);
    char* codeLengths = malloc(sizeof(char) * codesCount);
    for(int i=0; i<codesCount; i++){
        codeValues[i] = *pointer++;
        codeLengths[i] = *pointer++;
    }

    struct DecompressCode* first = malloc(sizeof(struct DecompressCode));
    first->isLast = 0;
    first->next[0] = 0;
    first->next[1] = 0;

    struct DecompressCode* currentCode;

    int bitIndex = 0;
    for(int i=0; i<codesCount; i++){
        currentCode = first;
        for(int j=0; j<codeLengths[i]; j++){
            char bit = getBit(pointer, bitIndex);
            bitIndex++;
            if(!currentCode->next[bit]){
                currentCode->next[bit] = malloc(sizeof(struct DecompressCode));
                currentCode->next[bit]->next[0] = NULL;
                currentCode->next[bit]->next[1] = NULL;
                currentCode->next[bit]->isLast = 0;
            }
            currentCode = currentCode->next[bit];
        }
        currentCode->isLast = 1;
        currentCode->value = codeValues[i];
    }
    free(codeValues);
    free(codeLengths);

    pointer += bitsToBytes(bitIndex);
    *dictLength = (int)(pointer-buffer);
    return first;
}

void readBytes(char* data, int length){
    length *= 8;
    for(int i=0; i < length; i++){
        printf("%c", getBit(data, i) + '0');
    }
    printf("\n");
}
void freeCode(struct DecompressCode* code){
    if(!code)
        return;
    freeCode(code->next[0]);
    freeCode(code->next[1]);
    free(code);
}
char* decompress(const char* compressedData, int* decompressedLength){
    const char* pointer = compressedData;

    int length = readInt(pointer);
    *decompressedLength = length;
    pointer += 4;

    int dictLength, codesCount;
    /*struct DecompressingCode* decompressingCodes = readDict(pointer, &dictLength, &codesCount);
    pointer += dictLength;

    char* data = malloc(length);

    int bitIndex = 0;
    for(int i=0; i<length; i++){
        int codeIndex = -1;
        int code = 0;
        int codeLength = 0;
        while(codeIndex == -1){
            char bit = getBit(pointer, bitIndex++);

            code = (code<<1) + bit;
            codeLength++;
            for(int j=0; j<codesCount; j++){
                if(decompressingCodes[j].code == code && decompressingCodes[j].codeLength == codeLength){
                    codeIndex = j;
                    break;
                }
            }
        }
        data[i] = decompressingCodes[codeIndex].value;
    }
    free(decompressingCodes);*/

    struct DecompressCode* firstDecompressCode = readDict(pointer, &dictLength, &codesCount);
    pointer += dictLength;

    char* data = malloc(length);

    int bitIndex = 0;
    for(int i=0; i<length; i++){
        int codeIndex = -1;
        int code = 0;
        int codeLength = 0;
        struct DecompressCode* currentCode = firstDecompressCode;
        while(1){
            char bit = getBit(pointer, bitIndex);
            bitIndex++;

            currentCode = currentCode->next[bit];
            if(!currentCode){
                printf("Error: wrong decompressing code\n");
                exit(1);
            }
            if(currentCode->isLast){
                data[i] = currentCode->value;
                break;
            }
            /*code = (code<<1) + bit;
            codeLength++;
            for(int j=0; j<codesCount; j++){
                if(decompressingCodes[j].code == code && decompressingCodes[j].codeLength == codeLength){
                    codeIndex = j;
                    break;
                }
            }*/
        }
        //data[i] = decompressingCodes[codeIndex].value;
    }
    //free(decompressingCodes);
    freeCode(firstDecompressCode);

    return data;
}

struct CompressedFile{
    char* data;
    int length;
    char* name;
};
int writeArchiveFile(char* archiveFileName, int filesCount, struct CompressedFile* compressedFiles){
    FILE* file = fopen(archiveFileName, "wb");
    if(file == NULL)
        return 1;

    fprintf(file, "ARC");
    fwrite(&filesCount, 4, 1, file);
    for(int i=0; i<filesCount; i++){
        int nameLength = strLength(compressedFiles[i].name);
        fwrite(compressedFiles[i].name, 1, nameLength+1, file);
        fwrite(&compressedFiles[i].length, 4, 1, file);
    }
    for(int i=0; i<filesCount; i++){
        fwrite(compressedFiles[i].data, 1, compressedFiles[i].length, file);
    }
    fclose(file);
    return 0;
}

int describeArchiveContent(char* archiveFileName){
    FILE* file = fopen(archiveFileName, "rb");
    if(file == NULL){
        printf("Error: could not open archive file\n");
        return 1;
    }

    int filesCount;
    fread(&filesCount, 1, 3, file);
    if(stringsCompare((char*)&filesCount, "ARC", 3)){
        printf("Error: specified file is not an archive file\n");
        fclose(file);
        return 1;
    }

    fread(&filesCount, 4, 1, file);

    for(int i=0; i<filesCount; i++){
        int currentIndex = ftell(file);
        char c = 1;
        while(c){
            fread(&c, 1, 1, file);
        }
        int nameLength = ftell(file) - currentIndex;
        char* fileName = malloc(nameLength);
        fseek(file, currentIndex, SEEK_SET);
        fread(fileName, 1, nameLength, file);

        int compressedLength;
        fread(&compressedLength, 4, 1, file);

        printf("%s -  %d bytes\n", fileName, compressedLength);
        free(fileName);
    }
    fclose(file);
    return 0;
}

int extractArchive(char* archiveFileName){
    FILE* file = fopen(archiveFileName, "rb");
    if(file == NULL){
        printf("Error: could not open archive file\n");
        return 1;
    }

    int filesCount;
    fread(&filesCount, 1, 3, file);
    if(stringsCompare((char*)&filesCount, "ARC", 3)){
        printf("Error: specified file is not an archive file\n");
        fclose(file);
        return 1;
    }

    fread(&filesCount, 4, 1, file);

    struct CompressedFile* compressedFiles = malloc(sizeof(struct CompressedFile) * filesCount);

    for(int i=0; i<filesCount; i++){
        int currentIndex = ftell(file);
        char c = 1;
        while(c){
            fread(&c, 1, 1, file);
        }
        int nameLength = ftell(file) - currentIndex;
        compressedFiles[i].name = malloc(nameLength);
        fseek(file, currentIndex, SEEK_SET);
        fread(compressedFiles[i].name, 1, nameLength, file);

        fread(&compressedFiles[i].length, 4, 1, file);
    }
    for(int i=0; i<filesCount; i++){
        char* compressedData = malloc(compressedFiles[i].length);
        fread(compressedData,  compressedFiles[i].length, 1, file);

        char* decompressedData = decompress(compressedData, &compressedFiles[i].length);

        writeFile(compressedFiles[i].name, compressedFiles[i].length, decompressedData);

        free(compressedData);
        free(decompressedData);
    }
    fclose(file);
    return 0;
}

void printUsage(){
    printf("    --file (archive name) (file1) [file2...]\n");
    printf("    --file (archive name) --list\n");
    printf("    --file (archive name) --extract\n");
}

int main(int argc, char** args) {

    char* file = NULL;
    enum Action{
        UNDEFINED,
        CREATE,
        EXTRACT,
        LIST
    };
    enum Action action = UNDEFINED;

    char* files[256];
    int filesCount = 0;

    for(int i=1; i<argc;i++){
        if(!stringsCompare(args[i], "--", 2)){
            if(!stringsCompare(args[i], "--file", 6)){
                if(i+1 == argc){
                    printf("Error: no string after --file\n");
                    return 1;
                }
                file = args[++i];
            } else if(!stringsCompare(args[i], "--create", 8)){
                action = CREATE;
            } else if(!stringsCompare(args[i], "--extract", 9)){
                action = EXTRACT;
            } else if(!stringsCompare(args[i], "--list", 8)){
                action = LIST;
                continue;
            } else {
                printf("Error: undefined command: %s\n", args[i]);
                return 1;
            }
        } else {
            files[filesCount++] = args[i];
        }
    }
    if(action == UNDEFINED)
        action = CREATE;

    if(file == NULL){
        printf("Error: archive file name is not defined\n");
        printUsage();
        return 1;
    }

    switch (action) {
        case CREATE:
            if(filesCount == 0){
                printf("Error: input files are not defined\n");
                printUsage();
                return 1;
            }
            struct CompressedFile* compressedFiles = malloc(sizeof(struct CompressedFile)*filesCount);
            for(int i=0; i<filesCount; i++){
                int length;
                char* fileData = readFile(files[i], &length);
                if(fileData == NULL){
                    for(int j=0; j<i; j++)
                        free(compressedFiles[j].data);
                    free(compressedFiles);
                    printf("Error: could not read file \"%s\"\n", files[i]);
                    return 1;
                }
                char* compressed = compress(fileData, length, &length);
                free(fileData);
                compressedFiles[i].data = compressed;
                compressedFiles[i].length = length;
                compressedFiles[i].name = files[i];
            }
            if(writeArchiveFile(file, filesCount, compressedFiles)){
                printf("Error: could not write archive file\n");
                for(int i=0; i<filesCount; i++)
                    free(compressedFiles[i].data);
                free(compressedFiles);
                return 1;
            }
            for(int i=0; i<filesCount; i++)
                free(compressedFiles[i].data);
            free(compressedFiles);
            printf("File was successfully written\n");
            break;
        case LIST:
            if(describeArchiveContent(file))
                return 1;
            break;
        case EXTRACT:
            extractArchive(file);
            break;
    }

    return 0;
}