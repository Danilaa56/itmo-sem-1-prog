#include <stdio.h>
#include <stdlib.h>

void showArray(char* array, int length){
    printf("%c",array[0]);
    for(int f=1;f<length;f++)
        printf("\t%c",array[f]);
    printf("\n");
}

int main() {

    //Task 1
    char array[4];
    for(int f='a';f<='d';f++){
        array[f-'a'] = f;
    }
    showArray(array,4);

    //Task 2
    char* array2 = malloc(sizeof(char)*4);
    for(int f='a';f<='d';f++){
        array2[f-'a'] = f;
    }
    showArray(array2,4);
    free(array2);

    return 0;
}
