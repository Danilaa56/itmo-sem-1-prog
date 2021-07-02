#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <mem.h>

#define W1251 0
#define UTF16 1

int areStringsEqual(const char* string1, const char* string2, int length){
    int index = 0;
    while (index!=length){
        if(string1[index]!=string2[index])
            return 0;
        if(string1[index]==0)
            return 1;
        index++;
    }
    return 1;
}
short* readUsualStringAsWideString(const char* data, int length, int* stringLength){
    short* str = malloc(length*2+2);
    str[length] = 0;

    for(int i=0; i<length; i++)
        str[i] = (unsigned char)data[i];

    if(stringLength != NULL)
        *stringLength = length;
    return str;
}
short* readWideString(const char* data, int length, int* stringLength){
    char isLittleEndian;
    if(data[0]==-1 && data[1]==-2) // xFF xFE
        isLittleEndian = 1;
    else if(data[0] == -2&&data[1]==-1)  // xFE xFF
        isLittleEndian = 0;
    else{
        printf("BAD UTF SIGNATURE: %x %x\n", data[0], data[1]);
        exit(0);
    }

    data += 2;
    length = (length-2)/2;

    short* str = malloc(length*2+2);
    str[length] = 0;

    if(isLittleEndian){
        for(int i=0; i<length; i++)
            str[i] = data[i*2] | (data[i*2+1]<<8);
    } else {
        for(int i=0; i<length; i++)
            str[i] = (data[i*2]<<8) | data[i*2+1];
    }
    if(stringLength != NULL)
        *stringLength = length;
    return str;
}
short* readT(const char* data, int length, int* stringLength){
    char encoding = data[0];
    if(encoding == 0){
        return readUsualStringAsWideString(data+1, length-1, stringLength);
    }else if(encoding==1){
        return readWideString(data+1, length-1, stringLength);
    }else{
        printf("Unsupported encoding: %d\n", encoding);
        exit(0);
    }
}
short* readCOMM(const char* data, int length, int* stringLength){
    char encoding = data[0];

    short* description;
    short* fullText;

    int descriptionLength;
    int fullTextLength;

    if(encoding == 0){
        const char* descriptionPointer = data+4;
        while(*descriptionPointer)
            descriptionPointer++;
        descriptionLength = descriptionPointer-(data+4);
        description = readUsualStringAsWideString(data+4, descriptionLength, NULL);
        fullText = readUsualStringAsWideString(data+4+descriptionLength+1, length-4-descriptionLength-1, &fullTextLength);
    }else if(encoding==1){
        if(data[4] || data[5]){
            const char* descriptionPointer = data+4;
            while(descriptionPointer[0] || descriptionPointer[1])
                descriptionPointer+=2;
            descriptionLength = descriptionPointer-(data+4);
        }else{
            description = malloc(2);
            description[0] = 0;
            descriptionLength = 0;
        }
        fullText = readWideString(data+6+descriptionLength, length-6-descriptionLength, &fullTextLength);
    }else{
        printf("Unsupported encoding: %d\n", encoding);
        exit(0);
    }

    *stringLength = fullTextLength;
    free(description);
    return fullText;
}

void stringCopy(char* dest, const char* src, int length){
    int i = 0;
    while (i!=length){
        dest[i] = src[i];
        if(src[i++]==0)
            break;
    }
}

struct ID3v2flagsField{
    unsigned unsynchronisation: 1;
    unsigned unsignedHeader: 1;
    unsigned experimentalIndicator: 1;
};
typedef union ID3v2flags{
    struct ID3v2flagsField field;
    char value;
} ID3v2flags;

struct ID3v2frameFlagsField{
    unsigned tagAlterPreservation: 1;
    unsigned fileAlterPreservation: 1;
    unsigned readOnly: 1;
    unsigned :5;
    unsigned compression: 1;
    unsigned encryption: 1;
    unsigned groupingIdentify: 1;
};
typedef union ID3v2frameFlags{
    struct ID3v2frameFlagsField field;
    unsigned short value;
} ID3v2frameFlags;

//typedef struct String{
//    short* chars;
//    int length;
//}String;

//String* createString(char* chars){
//    String* string = malloc(sizeof(String));
//    return string;
//}

void printWideString(const short* str){
    char* save_locale = setlocale(LC_CTYPE, "");
    int i = 0;
    int lastCode = W1251;
    setlocale(LC_ALL, "Russian_Russia.1251");
    while(str[i]!=0){
        int code = -1;
        short c = str[i];
        if(c<128){
            code = 0;
        }else if( 0x410 <= c && c <= 0x44f){
            c -= 0x350;
            code = 0;
        }else if( c == 0x451){
            c = 0xb8;
            code = 0;
        }else if( c == 0x401){
            c = 0xa8;
            code = 0;
        }
        if(code==-1){
            printf("WRONG SYMBOL CODE: %d", c);
            exit(0);
        }
        if(code!=lastCode){
            switch (code) {
                case W1251:
                    setlocale (LC_ALL, "Russian_Russia.1251");
                    break;
            }
            lastCode = code;
        }
        printf("%c",c);
        i++;
    }
    setlocale(LC_CTYPE, save_locale);
}

typedef struct sNode {
    struct sNode* prev;
    struct sNode* next;
    void* value;
}Node;
typedef struct sLinkedList{
    int size;
    Node* first;
    Node* last;
    void (*add)(struct sLinkedList* list, void* value);
    void* (*removeFirst)(struct sLinkedList* list);
} LinkedList;
void addIntoList(LinkedList* list, void* value){
    Node* node = malloc(sizeof(Node));
    node->next = NULL;
    node->value = value;
    node->prev = list->last;
    if(list->size++==0) {
        list->first = node;
    }else{
        list->last->next = node;
    }
    list->last = node;
}
void* removeNode(LinkedList* list, Node* nodeToRemove){
    Node* node = list->first;
    while(node){
        if(node==nodeToRemove){
            if(node->next){
                node->next->prev = node->prev;
            }else{
                list->last = node->prev;
            }
            if(node->prev){
                node->prev->next = node->next;
            }else{
                list->first = node->next;
            }
            list->size--;
            void* value = node->value;
            free(node);
            return value;
        }
        node = node->next;
    }
    return NULL;
}
void* removeFirst(LinkedList* list){
    return removeNode(list, list->first);
}
void removeValue(LinkedList* list, void* value){
    if(list->size==0)
        return;

    Node* node = list->first;
    while(node){
        if(node->value==value){
            removeNode(list, node);
        }
        node = node->next;
    }
}
LinkedList* createLinkedList(){
    LinkedList* list = malloc(sizeof(LinkedList));
    list->size = 0;
    list->first = NULL;
    list->last = NULL;
    list->add = addIntoList;
    list->removeFirst = removeFirst;
    return list;
}

struct Frame{
    char id[5];
    ID3v2frameFlags flags;
    int length;
    char* data;
};

char* getFrameName(const struct Frame* frame, int* strLength){
    if(areStringsEqual(frame->id, "COMM", 4)){
        char* str = malloc(9);
        memcpy(str, frame->id, 4);
        str[4] = '.';
        memcpy(str+5, frame->data+1, 3);
        str[8] = 0;
        *strLength = 8;
        return str;
    }else{
        char* str = malloc(5);
        memcpy(str, frame->id, 4);
        str[4] = 0;
        *strLength = 4;
        return str;
    }
}
short* getFrameText(const struct Frame* frame, int* strLength){
    if(frame->id[0]=='T'){
        return readT(frame->data, frame->length, strLength);
    }else if(areStringsEqual(frame->id,"COMM",4)){
        return readCOMM(frame->data, frame->length, strLength);
    }else{
        short* str = malloc(9*2);
        for(int i=0;i<8;i++)
            str[i] = (unsigned char)("<NoText>"[i]);
        str[8] = 0;
        *strLength = 8;
        return str;
    }
}

void printChars(char c, int count){
    for(int i=0; i<count; i++)
        printf("%c", c);
}
void showFrames(LinkedList* framesList){
    int framesCount = framesList->size;
    struct Frame* frames[framesCount];

    char** tagNames = malloc(sizeof(char*)*framesCount);
    short** values = malloc(sizeof(short*)*framesCount);

    int* tagNamesLength = malloc(sizeof(int*)*framesCount);
    int* valuesLength = malloc(sizeof(int*)*framesCount);

    int maxTagNamesLength = 3;
    int maxValuesLength = 5;

    {
        Node* node = framesList->first;
        for(int i=0; i<framesCount; i++){
            struct Frame* frame = node->value;

            tagNames[i] = getFrameName(frame, &tagNamesLength[i]);
            if(tagNamesLength[i] > maxTagNamesLength)
                maxTagNamesLength = tagNamesLength[i];
            values[i] = getFrameText(frame, &valuesLength[i]);
            if(valuesLength[i] > maxValuesLength)
                maxValuesLength = valuesLength[i];

            node = node->next;
        }
    }

    printf("+");
    printChars('-', 2 + maxTagNamesLength);
    printf("+");
    printChars('-', 2 + maxValuesLength);
    printf("+\n");

    printf("| Tag ");
    printChars(' ', maxTagNamesLength-3);
    printf("| Value ");
    printChars(' ', maxValuesLength-5);
    printf("|\n");

    printf("+");
    printChars('-', 2 + maxTagNamesLength);
    printf("+");
    printChars('-', 2 + maxValuesLength);
    printf("+\n");

    for(int i=0; i<framesCount; i++){
        printf("| ");
        printf("%s", tagNames[i]);
        printChars(' ', maxTagNamesLength - tagNamesLength[i]);
        printf(" | ");
        printWideString(values[i]);
        printChars(' ', maxValuesLength - valuesLength[i]);
        printf(" |\n");
        free(tagNames[i]);
        free(values[i]);
    }

    printf("+");
    printChars('-', 2 + maxTagNamesLength);
    printf("+");
    printChars('-', 2 + maxValuesLength);
    printf("+\n");

    free(tagNames);
    free(values);
    free(tagNamesLength);
    free(valuesLength);
}
void uppercase(char* str){
    while(*str){
        if(*str>=0x61 && *str<=0x7a){
            *str -= 0x20;
        }
        str++;
    }
}
int getLocaleNum(const char* localeName){
    printf("Locale name: %s\n", localeName);
    int n = 0;
    while(*localeName){
        if(*localeName>=48 && *localeName <= 57){
            n = n*10 + *localeName - 48;
        }
        localeName++;
    }
    return n;
}

short* stringToWideString(const char* inputString, int localeNum){
    short* str;
    int length;
    switch (localeNum) {
        case 1251:
            length = strlen(inputString);
            str = malloc(length*2 + 2);
            str[length] = 0;
            for(int i=0; i<length; i++){
                unsigned char c = inputString[i];
                if(c<128)
                    str[i] = c;
                else if( c>=0xc0 && c<=0xFF){
                    str[i] = 0x350+c;
                }else{
                    printf("WRONG SYMBOL CODE:: %d", c);
                    exit(0);
                }
            }
            return str;
        default:
            printf("Unsupported prompt locale: %d\n", localeNum);
            exit(0);
            break;
    }
}

short* inputStringToWideString(const char* inputString){
    int localeNum = getLocaleNum(setlocale(LC_CTYPE, NULL));
    return stringToWideString(inputString, localeNum);
}

struct Frame* getFrameByTag(LinkedList* frames, const char* tag){
    Node* node = frames->first;
    while(node!=NULL){
        struct Frame* frame = node->value;
        if(strcmp(frame->id,tag) == 0)
            return frame;
        node = node->next;
    }
    return NULL;
}

void save(const char* filePath, const char* buffer, int length, LinkedList* frames, ID3v2flags flags){
    FILE* file = fopen(filePath, "wb");
    fprintf(file, "ID3");           // ID3 header
    fprintf(file, "%c%c",4,0);      // ID3 version and subversion
    fprintf(file, "%c", flags);
    fseek(file, 4, SEEK_CUR); // skip header size

    int headerSize = 0;
    Node* node = frames->first;
    while(node){
        struct Frame* frame = node->value;

        fwrite(frame->id, 4, 1, file);

        fprintf(file, "%c%c%c%c", (frame->length>>21), (frame->length>>14)&0x7f, (frame->length>>7)&0x7f, (frame->length)&0x7f);
        fprintf(file, "%c%c", (frame->flags.value>>7), (frame->flags.value)&0x7f);
        fwrite(frame->data, frame->length, 1, file);
        headerSize += 10 + frame->length;

        node = node->next;
    }
    fwrite(buffer, length, 1, file);
    fseek(file, 6, SEEK_SET);
    fprintf(file, "%c%c%c%c", (headerSize>>21), (headerSize>>14)&0x7f, (headerSize>>7)&0x7f, (headerSize)&0x7f);

    fclose(file);
    printf("Saved\n");
}

void errorExit(char* msg){
    printf("%s",msg);
    exit(1);
}

long readFile(char* filePath, char** buffer){
    FILE* file = fopen(filePath, "rb");
    if(!file)
        errorExit("Couldn't open file\n");
    fseek(file, 0, SEEK_END);
    long fileLength = ftell(file);
    char* tmpBuffer = malloc(sizeof(char)*fileLength);
    if(!tmpBuffer)
        errorExit("Failed to allocate memory\n");
    rewind(file);
    fread(tmpBuffer, fileLength, 1, file);
    fclose(file);
    *buffer = tmpBuffer;
    return fileLength;
}

int main(int argc, char** args) {
    char* filePath = NULL;
    char isShow = 0;
    char* set = NULL;
    char* value = NULL;
    char* get = NULL;

    for(int i=1; i<argc; i++){
        if(areStringsEqual(args[i],"--filepath",10))
            filePath = args[i]+11;
        else if(areStringsEqual(args[i],"--show",6))
            isShow = 1;
        else if(areStringsEqual(args[i],"--set",5))
            set = args[i]+6;
        else if(areStringsEqual(args[i],"--value",7))
            value = args[i]+8;
        else if(areStringsEqual(args[i],"--get",5))
            get = args[i]+6;
    }

    if(filePath==NULL)
        errorExit("--filepath was not defined\n");
    //printf("File name: %s\n",filePath);

    char* buffer;
    long fileLength = readFile(filePath, &buffer);
    unsigned char* uBuffer = (unsigned char*)buffer;
    //printf("File length: %ld\n",fileLength);

    if(!areStringsEqual(buffer,"ID3",3))
        errorExit("File does not use ID3\n");

    char id3v2version = buffer[3];
    char id3v2subversion = buffer[4];
    ID3v2flags flags;
    flags.value = buffer[5];
    int size = (buffer[6]<<21) | (buffer[7]<<14) |(buffer[8]<<7) | buffer[9];

    printf("ID3v2 Version: %d.%d\n", id3v2version, id3v2subversion);
    printf("flags: %d\n", flags.value);
    printf("Header size: %d\n", size);

    if(flags.value!=0)
        errorExit("FLAGS IS NOT 0\n");
    if(id3v2subversion!=0)
        errorExit("id3v2subversion IS NOT 0\n");
    if(id3v2version!=4 && id3v2version!=3)
        errorExit("id3v2version IS NEITHER 4 NOR 3\n");

    unsigned long long index = 10;
    setlocale(LC_ALL, "");

    char frameIdLength = (id3v2version==2) ? 3 : 4;
    char frameId[frameIdLength+1];
    frameId[frameIdLength] = 0;
    union ID3v2frameFlags frameFlags;

    size += 10;

    LinkedList* frames = createLinkedList();

    while(index+6+frameIdLength<size){
        stringCopy(frameId, buffer+index, frameIdLength);
        index += frameIdLength;
        int frameSize = (uBuffer[index++]<<24) | (uBuffer[index++]<<16) |(uBuffer[index++]<<8) | uBuffer[index++];
        frameFlags.value = (uBuffer[index++]<<8) | uBuffer[index++];

        //printf("Index %lld\n", index);
        //printf("Frame ID: %s\nFrame length: %d\nFrame flags: %d\n", frameId, frameSize, frameFlags.value);

        if(frameSize){
            struct Frame* frame = malloc(sizeof(struct Frame));

            stringCopy(frame->id, frameId, 5);
            frame->length = frameSize;
            frame->flags = frameFlags;
            frame->data = malloc(frameSize);
            memcpy(frame->data, buffer+index, frameSize);

            frames->add(frames, frame);

            index += frameSize;
        }
    }
    if(isShow) {
        showFrames(frames);
        exit(0);
    }
    if(get){
        uppercase(get);
        struct Frame* frame = getFrameByTag(frames, get);
        if(frame) {
            printf("%s = ", get);
            printWideString(readT(frame->data, frame->length, NULL));
            printf("\n");
            exit(0);
        }
        printf("Field %s wasn't found\n", get);
        exit(0);
    }
    if(set){
        uppercase(set);
        if(!value){
            printf("Enter --value=argument\n");
            exit(0);
        }
        short* wideString = inputStringToWideString(value);
        struct Frame* frame = getFrameByTag(frames, set);
        if(strcmp(value, "NULL")==0){
            if(frame){
                free(frame->data);
                removeValue(frames, frame);
                printf("Tag %s was removed\n", set);
                save(filePath, buffer+size, fileLength-size, frames, flags);
            }else{
                printf("There is no tag %d\n", set);
            }
            exit(0);
        }
        int length = 0;
        while(wideString[length])
            length++;
        char* stringData = malloc(3+length*2);
        stringData[0] = 1;
        stringData[1] = -2; //Big endian
        stringData[2] = -1;
        for(int i=0; i<length; i++){
            stringData[3+i*2] = (wideString[i]>>8)&0xff;
            stringData[3+i*2+1] = wideString[i]&0xff;
        }
        if(!frame){
            frame = malloc(sizeof(struct Frame));
            memcpy(frame->id, set, frameIdLength);
            frames->add(frames,frame);
        }else{
            free(frame->data);
        }
        frame->data = stringData;
        frame->length = length*2+3;
        save(filePath, buffer+size, fileLength-size, frames, flags);
        exit(0);
    }
    return 0;
}