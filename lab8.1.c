#include <stdio.h>
#include <string.h>

int main() {
    char str1[100];
    char str2[100];

    scanf("%s %s", str1, str2);
    strcat(str1, str2);
    printf("%s\n", str1);

    return 0;
}
