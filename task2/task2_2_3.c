#include <stdio.h>
int main () {
    double x, a, integr, n;
    scanf("%lf %lf ", &n, &x);
    integr = 0;
    while (scanf("%lf ", &a) != EOF) {
        integr = a/(n+1);
        integr *= x;
        n--;
    }
    printf("\n%.10g\n", integr);
    return 0;
}
