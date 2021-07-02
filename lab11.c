#include <stdio.h>
#include "lab11Rect.h"

int main() {
    double x, y, width, height, angle;
    scanf("%lf %lf %lf %lf %lf",&x, &y, &width, &height, &angle);

    Rect* rect = createRect(x,y,width,height,angle);
    printf("Rectangle square is:    %lf\n",getRectSquare(rect));
    printf("Rectangle perimeter is: %lf\n",getRectPerimeter(rect));
    free(rect);

    return 0;
}