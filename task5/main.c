#include "func.c"
#include "lgragh.c"

int main(){
    signal(SIGINT,SIG_IGN);
    construct();
    return 0;
}