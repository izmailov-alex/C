#include <stdio.h>
int main () {
    double x, a0, der = 0, last_a = 0, temp = 0;
    scanf("%lf ", &x);
    der = 0;
    while (scanf("%lf ", &a0) != EOF) {
        der += temp;
        der *= x;
        der += last_a;
        temp *= x;
        temp += last_a;
        last_a = a0;
    }
    printf("\n%.10g\n",der);
    return 0;
}
