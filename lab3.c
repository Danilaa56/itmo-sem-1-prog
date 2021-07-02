#include <stdio.h>

int main() {

    //Task 1
    int num;
    scanf("%o",&num);

    //Task 2
    printf("%d\n",num);

    //Task 3
    printf("%x %x\n",num,num<<1);

    //Task 4
    printf("%x %x\n",num,~num);

    //Task 5
    int num2;
    scanf("%o",&num2);
    printf("%o\n",num & num2);

    return 0;
}