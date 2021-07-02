#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct Point{
    float x;
    float y;
} Point;

float** getRanges(Point** points, int length){
    float** ranges = malloc(sizeof(float*)*length);
    for(int i=0; i<length; i++){
        ranges[i] = malloc(sizeof(float)*(length-1));
        int index = 0;
        for(int j=0; j<length; j++){
            if(i==j)
                continue;
            ranges[i][index] = sqrt(pow(points[i]->x-points[j]->x,2)+pow(points[i]->y-points[j]->y,2));

            index++;
        }
    }
    return ranges;
}

int main() {
    int n;
    scanf("%d", &n);

    Point** points = malloc(sizeof(Point*)*n);

    for(int i=0;i<n;i++){
        float x, y;
        scanf("%f %f", &x, &y);
        Point* point = malloc(sizeof(Point));
        point->x = x;
        point->y = y;
        points[i] = point;
    }

    float** ranges = getRanges(points, n);

    for(int i=0; i<n; i++){
        int lastIndex = n-1;
        for(int j=0; j<lastIndex; j++){
            printf("%f ", ranges[i][j]);
        }
        printf("\n");
    }

    return 0;
}
