#include <stdio.h>
#include <stdlib.h>
#define N 5

void slesh(char *s){
    while(*s){
        if(*s == '\\'){
            *s++;
            switch (*s) {
                case 'a': printf("\a");
                    break;
                case 'b': printf("\b");
                    break;
                case 'c': exit(0);
                    break;
                case 'e': printf("\e");
                    break;
                case 'E': printf("\e");
                    break;
                case 'f': printf("\f");
                    break;
                case 'n': printf("\n");
                    break;
                case 'r': printf("\r");
                    break;
                case 't': printf("\t");
                    break;
                case '\\': printf("\\");
                    break;
                case 'v': printf("\v");
                    break;
                default: printf("\\%c", *s);
                    break;
            }
            *s++;
        } else {
            printf("%c", *s++);
        }
    }
}
int cmp(char *s1, char *s2){
    while(*s1 == *s2){
        s1++;
        s2++;
    }
    if (!*s2) return 1;
    return 0; 
}
int main(int argc, char **argv){
    int i=1,j, k=1;
    struct dict{
        char *s;
        int i;
    } flags[N] = {
        {"-n\0", 0},
        {"-e\0", 0},
        {"-E\0", 0},
        {"--help\0", 0},
        {"--version\0", 0},
    };
    if (argc == 1){
        printf("\n");
        return 0;
    }
    for(i=1; i<argc; i++){
        if (k && argv[i][0] == '-' && argv[i][1]!='\0'){
            for(j=0; j < N; j++){
                if(cmp(argv[i], flags[j].s)) flags[j].i++;
            }
        } else {
            k = 0;
            if(flags[1].i != 0 || flags[2].i != 0){
                slesh(argv[i]);
                if (i < argc-1) printf(" ");
            } else printf("%s%s", argv[i], (i < argc-1) ? " " : "");
        }
    }
    if(flags[0].i==0) printf("\n");
    return 0;
}