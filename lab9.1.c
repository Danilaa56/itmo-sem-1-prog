#include <stdio.h>

int main() {
    char str[100];

    scanf("%s",str);

    int upperCaseCount = 0;
    int lowerCaseCount = 0;
    int digitCount = 0;

    for(int i=0; str[i]!=0; i++){
        if( 47<str[i] && str[i] < 58){
            digitCount++;
            continue;
        }
        if( 64<str[i] && str[i] < 91){
            upperCaseCount++;
            continue;
        }
        if( 96<str[i] && str[i] < 123){
            lowerCaseCount++;
            continue;
        }
    }
    printf("Lower case characters: %d\n",lowerCaseCount);
    printf("Upper case characters: %d\n",upperCaseCount);
    printf("Digit characters: %d\n",digitCount);

    return 0;
}
