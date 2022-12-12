#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define N 100
int main() {
    char *buf = getenv("PWD");
    char *buff, *bufff;
    buff = (char*)malloc(N*sizeof(char));
    getcwd(buff, N*sizeof(char));
    printf("%s\n",buff);
    printf("%s\n",buf);
    free(buff);
    return 0;
}