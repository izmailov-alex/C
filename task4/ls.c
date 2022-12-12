#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <stdlib.h>
#define N 3
struct dict{
        char *s;
        int i;
};

char* uid_to_name(uid_t uid){
    //struct passwd * getpwuid(), *pw_ptr;
    struct passwd *pw_ptr;
    static char numstr[10]={0,};

    if ((pw_ptr = getpwuid(uid)) == NULL){
        sprintf(numstr,"%d", uid);
        return numstr;
    }else{
        return pw_ptr->pw_name;
    }
}

char* gid_to_name(gid_t gid){
    //struct group *getgrgid(), *grp_ptr;
    struct group *grp_ptr;
    static char numstr[10];
    
    if((grp_ptr = getgrgid(gid)) == NULL){
        sprintf(numstr,"%d", gid);
        return numstr;
    }else{
        return grp_ptr->gr_name;
    }
}
void mode_to_letters(int mode, char str[]){
    strcpy(str,"----------");

    if (S_ISDIR(mode)) str[0] = 'd';
    if (S_ISCHR(mode)) str[0] = 'c';
    if (S_ISBLK(mode)) str[0] = 'b';

    if (mode & S_IRUSR) str[1] = 'r';
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_IXUSR) str[3] = 'x';

    if (mode & S_IRGRP) str[4] = 'r';
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_IXGRP) str[6] = 'x';

    if (mode & S_IROTH) str[7] = 'r';
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_IXOTH) str[9] = 'x';

}
void printfile(char *s, struct dict *flags){
    struct stat stbuf;
    char modestr[11];
    stat(s, &stbuf);
    mode_to_letters(stbuf.st_mode, modestr);
    printf("%s", modestr);
    printf("%4d ", (int)stbuf.st_nlink);
    printf("%-8s ", uid_to_name(stbuf.st_uid));
    if(flags[2].i) printf("%-8s ", gid_to_name(stbuf.st_gid));
    printf("%8ld ", (long) stbuf.st_size);
    printf("%.12s ", 4 + ctime(&stbuf.st_mtime));
    printf("%s\n", s);
}

int walkread(DIR *a, struct dict *flags, char *buf){
    struct dirent *d;
    struct stat stbuf;
    char modestr[11];
    int flag=0, len;
    char w[20]={0,};
    DIR *b;
    if (a == NULL) {
        printf("cannot access\n");
        closedir(a);
        return -1;
    }
    while(d = readdir(a)){
        if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        if(d->d_type&DT_DIR) flag = 1;
        if(flags[2].i){
            printfile(d->d_name, flags);
        } else if(flags[1].i){
            printfile(d->d_name, flags);
        } else printf("%s ", d->d_name);
    }
    printf("\n");
    closedir(a);
    if(flags[0].i && flag){
        b = opendir(".");
        while(d = readdir(b)){
            if(strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
            if(d->d_type&DT_DIR){
                chdir(d->d_name);
                printf("\n%s%s:\n",buf,d->d_name);
                stpcpy(w, buf);
                len = strlen(w);
                for(int j=0; j<strlen(d->d_name); j++) w[len++] = d->d_name[j];
                w[len] = '/';
                walkread(opendir("."),flags, w);
                memset(w,0,len+1);
                chdir("..");
            }
        }
        closedir(b);
    }
    return 0;
}
int main(int argc, char**argv){
    DIR *a;
    static char buf[20]={0,};
    buf[0] = '.';
    buf[1] = '/';
    int i,j, k = 1, count_flag = 0;
    struct dict flags[N] = {
        {"-R\0", 0},
        {"-l\0", 0},
        {"-g\0", 0},
    };
    struct dirent *d;
    if(argc == 1){
        a = opendir(".");
        walkread(a, flags, buf);
        //printf("\n");
    } else {  
        for(i = 1; i<argc; i++){
            if (k && argv[i][0] == '-' && argv[i][1]!='\0'){
                count_flag++;
                for(j=0; j < N; j++){
                    if(!strcmp(argv[i], flags[j].s)) flags[j].i++;
                }
            } else {
                k = 0;
                if(argc-1 - count_flag == 1) {
                    printf("%s:\n", argv[i]);
                    if(chdir(argv[i])==-1){
                        fprintf(stderr,"error chdir %s\n", argv[i]);
                        exit(1);
                    }
                    walkread(opendir("."), flags, buf);
                    if(chdir("..")==-1){
                        fprintf(stderr,"error chdir %s\n", argv[i]);
                        exit(1);
                    }
                } else{
                    printf("%s:\n", argv[i]);
                    if(chdir(argv[i])==-1){
                        fprintf(stderr,"error chdir %s\n", argv[i]);
                        exit(1);
                    }
                    walkread(opendir("."), flags, buf);
                    if(strcmp(argv[i], ".")){ 
                        if(chdir("..")==-1){
                            fprintf(stderr,"error chdir %s\n", argv[i]);
                            exit(1);
                        }
                    }
                    printf("\n");
                }
            }
        }
        if(k==1){
            if(flags[0].i) printf(".:\n");
            walkread(opendir("."), flags, buf);
        }
    }
    return 0;
}