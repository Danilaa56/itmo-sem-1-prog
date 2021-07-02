#ifndef PROGA_FILES_H
#define PROGA_FILES_H
char* readFile(const char* filePath, int* length);
int writeFile(const char* filePath, int length, void* buffer);
#endif
