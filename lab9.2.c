#include <stdio.h>

int main() {
    int minuteCount;
    float monthPrice;
    float minutePrice;

    scanf("%d %f %f",&minuteCount, &monthPrice, &minutePrice);

    int bill = monthPrice;
    if(minuteCount>499)
        bill += (minuteCount-499)*minutePrice;
    printf("%d", bill);

    return 0;
}
