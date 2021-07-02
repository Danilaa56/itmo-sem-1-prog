#ifndef PROGA_LAB11RECT_H
#define PROGA_LAB11RECT_H
#ifndef _STDLIB_H_
#define _STDLIB_H_
#include <stdlib.h>
#endif
typedef struct Rect{
    double x, y, width, height, angle;
} Rect;
double getRectSquare(Rect* rect);
double getRectPerimeter(Rect* rect);
Rect* createRect(double x, double y, double width, double height, double angle);
#endif
