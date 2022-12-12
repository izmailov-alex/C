#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
int main(int argc, char **argv){
    int fd1, fd2, n, CREAT=0;
    struct stat *stbuf1=NULL, *stbuf2=NULL;
    char buf[BUFSIZ];
    if(argc == 1){
        printf("%s: missing file operand\nTry '%s --help' for more information.", argv[0], argv[0]);
        exit(1);
    }
    if(argc == 2){
        printf("%s: missing destination file operand after '%s'\n", argv[0], argv[1]);
        exit(1);
    }
    if(argc == 3){
        if(stat(argv[1],stbuf1) == -1){
            printf("%s: cannot stat '%s': No such file or directory\n", argv[0], argv[1]);
            exit(1);
        }
        if(stat(argv[2], stbuf2) == -1){
            if(errno == ENOENT){
                fd2 = creat(argv[2],S_IRWXU | S_IRWXG | S_IRWXO);
                CREAT = 1;
                close(fd2);
            }else{
                printf("%s: cannot stat '%s': No such file or directory\n", argv[0], argv[2]);
                exit(1);
            }
        } else CREAT=1;
        if(CREAT){
            if(stbuf1->st_ino == stbuf2->st_ino && stbuf1->st_dev == stbuf2->st_dev){
                printf("%s: '%s' and '%s' are the same file\n", argv[0], argv[1], argv[2]);
                exit(0);
            }
            if((fd1 = open(argv[1], O_RDONLY, stbuf1->st_mode)) == -1){
                printf("error opening file %s: %s\n", argv[1], strerror(errno));
                exit(1);
            }
            if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) == -1){
                printf("error opening file %s: %s\n", argv[2], strerror(errno));
                exit(1);
            }
            while((n = read(fd1, buf, BUFSIZ))>0){
                write(fd2,buf,n);
            }
            chmod(argv[2], stbuf1->st_mode);
            close(fd1);
            close(fd2);
        }
    }
    return 0;
}