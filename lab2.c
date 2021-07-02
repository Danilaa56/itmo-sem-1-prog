#include <stdio.h>
#include <math.h>

int main() {
    double alpha;
    scanf("%lf", &alpha);
    double z1 = 2*pow(sin(3*M_PI-2*alpha),2)*pow(cos(5*M_PI+2*alpha),2);
    printf("%lf\n",z1);
    double z2 = 0.25-0.25*sin(2.5*M_PI-8*alpha);
    printf("%lf\n",z2);
    return 0;
}