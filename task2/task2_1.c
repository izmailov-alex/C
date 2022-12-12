#include <stdio.h>
#include <stdlib.h>

int main () {
    double x, x1, x2, e;
    scanf("%lf ", &e);
    while (scanf("%lf ", &x) != EOF) {
        x1 = 1;
        x2 = 0.5*(x1 + x/x1);
        while (abs(x1 - x2) >= e) {
            x1 = x2;
            x2 = 0.5*(x1 + x/x1);
        }
        printf("\n%.10g", x2);
    }
    printf("\n");
    return 0;
}
