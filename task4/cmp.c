#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char **argv){
    int fd1, fd2, n1, n2, cline=1, cchar=0;
    char a1, a2;
    struct stat stbuf;
    if(argc == 3){
        if(argv==NULL ||argv[1] == NULL || stat(argv[1], &stbuf) == -1){
            printf("%s: cannot stat '%s': No such file or directory\n", argv[0], argv[1]);
            exit(1);
        } else{
            if((fd1 = open(argv[1], O_RDONLY, stbuf.st_mode)) == -1){
                printf("error opening file %s\n", argv[1]);
                exit(1);
            } else{
                if(stat(argv[2], &stbuf) == -1){
                    printf("%s: cannot stat '%s': No such file or directory\n", argv[0], argv[2]);
                    exit(1);
                } else {
                    if((fd2 = open(argv[2], O_RDONLY, stbuf.st_mode)) == -1){
                        printf("error opening file %s\n", argv[2]);
                        exit(1);
                    } else {
                        n1 = read(fd1, &a1, 1);
                        n2 = read(fd2, &a2, 1);
                        while(n1 !=0 && n2 !=0 && a1 == a2){
                            if(a1 == '\n') cline++;
                            cchar++;
                            n1 = read(fd1, &a1, 1);
                            n2 = read(fd2, &a2, 1);
                        }
                        if(n1 != n2 || a1 != a2){
                            printf("%s %s differ: char %i, line %i\n", argv[1], argv[2], cchar+1, cline);
                        }
                        close(fd1);
                        close(fd2);
                    }
                }
            }
        }
    }
    return 0;
}