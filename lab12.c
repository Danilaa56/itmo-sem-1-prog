#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** args) {
    if(argc!=2){
        printf("Start with one argument: fullFilePath\n");
        exit(0);
    }
    FILE* file = fopen(args[1], "w");

    time_t t = time(NULL);
    for(int f=0;f<10;f++){
        struct tm tm = *localtime(&t);
        if(f==0)
            fprintf(file, "%02d.%02d.%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        else
            fprintf(file, "\n%02d.%02d.%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
        fflush(file);
        t += 60*60*24;
    }
    fclose(file);

    return 0;
}