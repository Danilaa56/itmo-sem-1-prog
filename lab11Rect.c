#include "lab11Rect.h"
double getRectSquare(Rect* rect){
    return rect->width*rect->height;
}
double getRectPerimeter(Rect* rect){
    return (rect->width+rect->height)*2;
}
Rect* createRect(double x, double y, double width, double height, double angle){
    Rect* rect = malloc(sizeof(Rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->angle = angle;
    return rect;
}