#include <stdio.h>

int main()
{
    char str[128];
    scanf("%s",str);

    char c = str[0];
    int num = 0;
    int isNegative = 0;
    int isNum = 1;
    if(c=='-'){
        isNegative = 1;
    }
    int index = isNegative;
    while(str[index]!=0){
        c = str[index];
        if(c<48||c>57){
            isNum = 0;
            break;
        }
        num = num*10+(c-48);
        index++;
    }

    if(isNum){
        if(isNegative)
            num*=-1;
        printf("%d",num);
    }else{
        printf("%s",str);
    }

    return 0;
}