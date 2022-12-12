#include <stdio.h>

double str2double(char *ch) { 
    double l, temp, step, i;
    enum vertex {start, num, num_with_point, E, num_with_E_plus, num_with_E_minus}
    state = start;
    while(*ch != '\0') {
        switch (state){
            case start:{
                if ((*ch >= '0') && (*ch <= '9')){
                    state = num;
                    l = (*ch) - '0';
                    step = 10;
                    temp = 0;
                }
                break;
            }
            case num:{
                if (*ch == '.'){
                    step = 0.1;
                    state = num_with_point;
                } else if((*ch >= '0') && (*ch <= '9')) {
                    l = l*10 + ((*ch) - '0');
                } else if((*ch == 'E') || (*ch == 'e')){
                    state = E;
                }
                else state = start;
                break;
            }
            case num_with_point:{
                if ((*ch == 'E') || (*ch == 'e')) state = E;
                else if ((*ch >= '0') && (*ch <= '9')){
                    l = l + ((*ch) - '0')*step;
                    step = step/10;
                } else state = start;
                break;
            }
            case E:{
                if (*ch == '+') {state = num_with_E_plus;}
                else if (*ch == '-') {state = num_with_E_minus;}
                else state = start;
                break;
            }
            case num_with_E_minus:
            case num_with_E_plus:{
                if ((*ch >= '0') && (*ch <= '9')){
                    temp = temp*10 + ((*ch) - '0');
                } else state = start;
                break;
            }
        }
        ch++;
    }
    if (temp != 0){
        if (state == num_with_E_plus){
            for(i=0; i < temp; i++) l = l*10;
        } else {
            for(i=0; i < temp; i++) l = l/10;
        }
    }
    return l;
}

int main() {
    char x[10];
    while(scanf("%s", x) != EOF) {
        printf("%.10g\n\n", str2double(x));
    }
    return 0;
}
