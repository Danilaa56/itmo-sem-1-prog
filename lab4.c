#include <stdio.h>

int main() {
    //Task 1
    int num;
    scanf("%d",&num);
    printf("%s\n",((num>=1) && (num<=10) ? "Yes" : "No"));

    //Task 2
    scanf("%d",&num);
    printf("%d\n",num&1);
    return 0;
}
