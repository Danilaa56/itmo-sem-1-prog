#include <stdio.h>
#include <string.h>

int main() {
    char str1[100];
    char str2[100];

    scanf("%s %s", str1, str2);

    int difference = strcmp(str1,str2);
    printf("\"%s\" %s \"%s\"\n", str1, (difference<0) ? "goes before" : difference==0 ? "is equal" : "goes after", str2);

    return 0;
}
