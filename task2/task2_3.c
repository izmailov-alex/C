#include <stdio.h>
int recursive(int i) {
    if (i <= 0) return 0;
    if (i == 1) return 1;
    return recursive(i-2) + recursive(i-1);
}

int iterative(int i) {
    int f0, f1, f, sum, k;
    f0 = 0;
    f1 = 1;
    sum = 0;
    for(k = 0; k < i - 1; k++){
        sum = f0 + f1;
        f0 = f1;
        f1 = sum;
    }
    return (i==1?f1:sum);
}

int main () {
    int i;
    while(scanf("%i\n", &i) != EOF){
        printf("\n%i = %i", iterative(i), recursive(i));
    }
    printf("\n");
    return 0;
}
