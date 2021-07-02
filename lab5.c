#include <stdio.h>
#include <stdlib.h>

#define MATRIX_MEMBER_TYPE int

typedef struct Matrix{
    int width;
    int height;
    MATRIX_MEMBER_TYPE* array;
} Matrix;

Matrix* createMatrix(int width, int height, MATRIX_MEMBER_TYPE* array){
    Matrix* m = malloc(sizeof(Matrix));
    m->width = width;
    m->height = height;
    m->array = array;
    return m;
}

void showArray(MATRIX_MEMBER_TYPE* array, int length){
    printf("%d",array[0]);
    for(int f=1;f<length;f++)
        printf("\t%d",array[f]);
    printf("\n");
}

void showMatrix(Matrix* m){
    for(int f=0;f<m->height;f++){
        showArray(&(m->array[m->width*f]),m->width);
    }
}

Matrix* multiplyMatrix(Matrix* m1, Matrix* m2){
    if(m1->width!=m2->height)
        return NULL;
    Matrix* m3 = malloc(sizeof(Matrix));
    m3->height = m1->height;
    m3->width = m2->width;
    m3->array = malloc(sizeof(MATRIX_MEMBER_TYPE) * m3->width * m3->height);
    MATRIX_MEMBER_TYPE tmp;
    for(int row1=0; row1<m1->height; row1++){
        for(int column2=0; column2<m2->width; column2++){
            tmp = 0;
            for(int i=0; i<m1->width; i++){
                tmp += m1->array[row1*m1->width + i] * m2->array[i*m2->width + column2];
            }
            m3->array[row1*m2->width + column2] = tmp;
        }
    }
    return m3;
}

int main() {
    int array[] = {10,20,30,40,50,60,70};
    showArray(array, 7);

    MATRIX_MEMBER_TYPE* matrix_content = malloc(sizeof(MATRIX_MEMBER_TYPE)*4);
    matrix_content[0] = 1;   matrix_content[1] = 2;
    matrix_content[2] = 3;   matrix_content[3] = 4;
    Matrix* matrix1 = createMatrix(2,2, matrix_content);

    matrix_content = malloc(sizeof(MATRIX_MEMBER_TYPE)*4);
    matrix_content[0] = 1;   matrix_content[1] = 0;
    matrix_content[2] = 0;   matrix_content[3] = 1;
    Matrix* matrix2 = createMatrix(2,2, matrix_content);

    Matrix* matrix3 = multiplyMatrix(matrix1, matrix2);
    showMatrix(matrix3);

    return 0;
}
