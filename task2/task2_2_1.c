#include <stdio.h>
int main () {
    double x, a0, sum;
    scanf("%lf ", &x);
    sum = 0;
    while (scanf("%lf ", &a0) != EOF) {
        sum = x*sum + a0;
    }
    printf("\n%.10g\n",sum);
    return 0;
}
