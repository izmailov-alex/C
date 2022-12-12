#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>  
#include <string.h>
int main(int argc, char **argv){
    int fd, i, flag = 0, num = 1, n;
    struct stat stbuf;
    char a;
    if(argc == 1){
        while((n = read(1,&a,1)) > 0) printf("%c", a);
    } else {
        if(argv[1][0] == '-' && argv[1][1] != '\0'){
            if(strcmp(argv[1], "-n\0") == 0){
                flag = 1;
            } else{
                printf("%s: invalid option -- '%c'\nTry '%s --help' for more information.\n", argv[0], argv[1][1], argv[0]);
                exit(1);
            }
        }
        if(argc == 2 && (flag || (argv[1][0]=='-' && argv[1][1] == '\0'))){
            if(flag && num == 1) printf("     %i  %c", num, a);
            while((n = read(1,&a,1)) > 0){
                if(a == '\n'){
                    printf("\n");
                    num++;
                    if(flag && a!=EOF) printf("     %i  ", num);
                }else printf("%c", a);
            }
        }else{
            for(flag?(i = 2):(i = 1); i<argc; i++){
                if(stat(argv[i], &stbuf) == -1){
                    printf("%s: cannot stat '%s': No such file or directory\n", argv[0], argv[i]);
                    exit(1);
                }
                if((fd = open(argv[i], O_RDONLY, stbuf.st_mode)) == -1){
                    printf("error opening file %s\n", argv[i]);
                    exit(1);
                }
                n = read(fd, &a, 1);
                if(n){
                    if(flag && num == 1) printf("     %i  %c", num, a);
                    else printf("%c", a);
                    while((n = read(fd, &a, 1)) > 0){
                        if(a == '\n'){
                            printf("\n");
                            num++;
                            if(flag) printf("     %i  ", num);
                        }else printf("%c", a);
                    }
                }
            }
        }
    }
    return 0;
}