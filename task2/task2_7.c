#include<stdio.h>
#include<setjmp.h>
jmp_buf begin; 

char curlex;

void getlex(void);

int expr(int *count_scob, int *max_scob);

int add(int *count_scob, int *max_scob);

int mult(int *count_scob, int *max_scob);

void error();

int main()
{
    int count_scob=0, max_scob=0;
    int result;
    setjmp(begin);
    printf("==>");
    getlex();
    result = expr(&count_scob, &max_scob);
    if (curlex != '\n') error();
    printf("\n%d\n",result);
    printf("%i\n", max_scob);
    return 0;
}

void getlex()
{
    while ((curlex = getchar()) == ' ');
}

void error(void)
{
    printf("\nERROR!\n");
    while(getchar()!='\n');
    longjmp(begin,1);
}

int expr(int *count_scob, int *max_scob)
{
    int e = add(count_scob, max_scob);
    while (curlex == '+' || curlex == '-'){
        if (curlex == '+'){
            getlex();
            e += add(count_scob, max_scob);
        } else {
            getlex();
            e -= add(count_scob, max_scob);
        }
        
    }
    return e;
}

int add(int *count_scob, int *max_scob)
{
    int a = mult(count_scob, max_scob), d;
    while (curlex == '*' || curlex == '/'){
        if (curlex == '*'){
            getlex();
            a *= mult(count_scob, max_scob);
        } else {
            getlex();
            d = mult(count_scob, max_scob);
            if(d == 0){
                error();
            }
            a /= d;
        }
        
    }
    return a;
}

int mult(int *count_scob, int *max_scob)
{
    int m, power, temp;
    char last = curlex;
    getlex();
    switch(last){
    case '0': case '1': case '2': case '3': case '4': case '5':
    case '6': case '7': case '8': case '9': m = last-'0';
        if (curlex != '^') break;
        getlex();
        
        // power - степень
        power = mult(count_scob, max_scob);
          if (power < 0) {
            error();
        } if (power == 0) {
            m = 1;
        } else {
            temp = m;
            while(--power) m *= temp;
        }
        break;
    case '(': 
        (*count_scob)++;
        m = expr(count_scob, max_scob);
        if (curlex == ')') {
            if(*count_scob > *max_scob) {
                *max_scob = *count_scob;
                *count_scob = 0;
            }
            getlex();
            if (curlex != '^')break;
            getlex();
            power = mult(count_scob, max_scob);
            if (power < 0) error();
            if (power == 0) m = 1;
            else {
                temp = m;
                while(--power) m *= temp;
            }
            break;
        }
        default : error();
    }
    
    return m;
}

