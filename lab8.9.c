#include <stdio.h>
#include <string.h>

int main() {
    char str[100];
    char character;

    scanf("%s %c", str, &character);

    long long int lastIndex = strlen(str);
    while(lastIndex!=-1){
        if(str[lastIndex]==character)
            break;
        lastIndex--;
    }
    printf("'%c' is last seen at index %lld\n", character, lastIndex);

    return 0;
}
