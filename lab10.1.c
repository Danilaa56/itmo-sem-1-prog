#include <stdio.h>

int nod(int a, int b){
    if(a==b){
        return a;
    }
    if(a>b){
        if(a%b==0)
            return b;
        return nod(a-b,b);
    }
    if(b>a){
        if(b%a==0)
            return a;
        return nod(b-a,a);
    }
    if(a==b){
        return a;
    }
}

int nok(int a, int b){
    return a*b/nod(a,b);
}

int main() {
    int a;
    int b;

    scanf("%d %d", &a, &b);
    printf("HOD: %d\n", nod(a,b));
    printf("HOK: %d\n", nok(a,b));

    return 0;
}
