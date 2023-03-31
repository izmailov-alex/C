#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#define SIZE 16
#define N 10
int *activeproc = NULL;
int curactive = 0;
int *backproc = NULL;
int curback = 0;

typedef struct cmd_inf {
    char ** argv; // список из имени команды и аргументов
    char *infile; // переназначенный файл стандартного ввода
    char *outfile; // переназначенный файл стандартного вывода
    int backgrnd; // =1, если команда подлежит выполнению в фоновом режиме
    struct cmd_inf* psubcmd; // команды для запуска в дочернем shell
    struct cmd_inf* pipe; // следующая команда после “|”
    struct cmd_inf* next; // следующая после “;” (или после “&”)
    int append; // 0 - ">", 1 - ">>"
    int flag; // 3 - "&&", 4 - "||"
    int error; // если комманда выполнилась с ошибкой
} cmd;

/*Работа со списком слов*/

//Приглашение к вводу
void inv() {
    printf("%s", "\x1b[32m"); /*здесь изменяется цвет на зеленый */
    char s[100]; /* ограничение: имя хоста и текущей директории не должно быть слишком длинным! */
    gethostname(s, 100);
    printf("%s@%s", getenv("USER"), s);
    fflush(stdout);
    printf("%s", "\x1B[37m"); /* здесь изменяется цвет на серый */
    getcwd(s, 100);
    printf(":%s$ ", s);
    fflush(stdout);
}

//очищаем буфер
void null_buf(char *buf, int *sizebuf, int *curbuf){
    buf = NULL;
    *sizebuf = 0;
    *curbuf = 0;
}

//добавляем символ в буфер
void addsym(int c, char **buf, int *sizebuf, int *curbuf){
    if (*curbuf > (*sizebuf)-1){
        *buf = realloc(*buf, (*sizebuf)+=SIZE);
    }
    (*buf)[(*curbuf)++] = c;
}

//проверка на специальный символ
int check1(int c){
    return c!='|' && c!='>' && c!='&' && c!='<' && c!='(' && c!=')' && c!=';';
}

//проверка на повторные специальные символы
int check2(int c){
    return c=='>' || c=='|' || c=='&';
}

//проверка не печатающиеся символы
int symset(int c){
    return (c!='\0') && (c!=' ') && (c!='\t') && (c!=EOF) ;
}

//добавление слова в список
void addword(char **buf, int *sizebuf, int *curbuf, char ***mass, int *sizelist, int *curlist){
    char **temp;
    if (*curbuf>(*sizebuf)-1){
        *buf = realloc(*buf, (*sizebuf)+1);
    }
    (*buf)[(*curbuf)++] = '\0';
    *buf = realloc(*buf, *sizebuf=*curbuf);
    if(*curlist > (*sizelist)-1){
        temp = realloc(*mass, ((*sizelist)+=SIZE)*sizeof(*mass));
        if (temp != NULL) *mass = temp;
        else printf("Произошла ошибка\n");
    }
    (*mass)[(*curlist)++] = *buf;
}

//функция обновления буфера ввода
int upd(int *curinp, char *inp){
    char a;
    int k;
    if ((k = fscanf(stdin, "%15[^\n]", inp)) == EOF) {free(inp); return 1;}
    scanf("%c", &a);
    if (a != '\n') inp[15] = a;
    if (k == 0) inp[0] = '\0';
    *curinp = 0;
    return 0;
}

//функция считывания нового символа
int getsym(int *curinp, char *inp){
    if (*curinp == SIZE){
        if (upd(&*curinp, inp)) return EOF;
    }
    return (inp)[(*curinp)++];
}
//инициализация списка
void null_list(char **mass, int *sizelist, int *curlist){
    mass = NULL;
    *sizelist = 0;
    *curlist = 0;
}
//очищение списка
void clearlist(char **mass, int *sizelist, int *curlist){
    int i;
    if (mass != NULL){
        for(i=0; i<*sizelist-1; i++){ 
            free(mass[i]);
        }
        free(mass[i]);
        free(mass);
        mass = NULL;
    }
    *curlist = 0;
    *sizelist = 0;
}

//корректировка размеров списка
void termlist(char ***mass, int *sizelist, int *curlist){
    if (*mass != NULL){
        if (*curlist>(*sizelist)-1) *mass = realloc(*mass, ((*sizelist)+1)*sizeof(**mass));
        (*mass)[*curlist] = NULL;
        *sizelist = (*curlist)+1;
        *mass = realloc(*mass, ((*sizelist)*sizeof(**mass)));
        (*mass)[*sizelist-1] = NULL;
    }
}
//печать списка
void printlist(char ***mass, int *sizelist){
    int i;
    if (*mass != NULL){
        for(i = 0; i<(*sizelist)-1; i++){
            printf("%s-" , (*mass)[i]);
        }
    }
    printf("\n");
}

/* Работа с командами и процессами*/

//проверка на специальный символ
int check(char *s){
    if(!strcmp(s,";\0")) return 1;
    if(!strcmp(s,"&\0")) return 2;
    if(!strcmp(s,"&&\0")) return 3;
    if(!strcmp(s,"||\0")) return 4;
    if(!strcmp(s,">\0")) return 5;
    if(!strcmp(s,"<\0")) return 6;
    if(!strcmp(s,">>\0")) return 7;
    if(!strcmp(s,"|\0")) return 8;
    return 0;
}
// Создание команды
cmd* creatcom(void) {
    cmd *temp;
    temp = (cmd*)malloc(sizeof(cmd)+1);
    if(temp==NULL){
        printf("error: malloc\n");
        return NULL;
    }
    temp->argv = NULL;
    temp->infile = NULL;
    temp->outfile = NULL;
    temp->backgrnd = 0;
    temp->psubcmd = NULL;
    temp->pipe = NULL;
    temp->next = NULL;
    temp->append = 0;
    temp->flag = 0;
    temp->error = 0;
    return temp;
}

//Печать структуры
void printfstruct(cmd *com){
    if(com->argv == NULL) return;
    while(com != NULL){
        printf("argv = %s\n", com->argv[0]);
        printf("infile = %s\n",com->infile);
        printf("outfile = %s\n", com->outfile);
        printf("backgrnd = %i\n", com->backgrnd);
        printf("psubcmd = %s\n",(com->psubcmd==NULL)?"NULL\0":com->psubcmd->argv[0]);
        printf("pipe = %s\n",(com->pipe==NULL)?"NULL\0":com->pipe->argv[0]);
        printf("next = %s\n",(com->next==NULL)?"NULL\0":com->next->argv[0]);
        printf("append = %i\n", com->append);
        switch (com->flag){
            case 3: printf("flag = &&\n"); break;
            case 4: printf("flag = ||\n"); break;
        }
        printf("\n");
        if(com->next == NULL) com = com->pipe;
        else com = com->next;
    }
}

//обновление таблицы внешних процессов
void updateback(){
    int j=0, count=0 , i=0;
    while(i<curback){
        if(!backproc[i]){
            j = i+1;
            while(j<curback && backproc[j]==0) j++;
            if(j>=curback) break;
            backproc[i] = backproc[j];
            backproc[j] = 0;
        }
        count++;
        i++;
    }
    curback = count;
    backproc = (int*)realloc(backproc,sizeof(int)*curback);
}

//Завершение внешних процессов
void hunt(){
    int status, i;
    for(i = 0; i<curback; i++){
        status = waitpid(backproc[i],NULL, WNOHANG);
        if(status){
            if (i==curback-1) printf("[%d] %d Last Done\n", i+1, backproc[i]);
            else printf("[%d] %d Done\n", i+1, backproc[i]);
			backproc[i] = 0;
        }else printf("[%d] %d running\n", i+1, backproc[i]);
    }
    updateback();
}

//Очистка процессов
void cleanprocess(){
    for(int i=0; i<curactive; i++) waitpid(activeproc[i],NULL,0);
    curactive = 0;
    free(activeproc);
    activeproc = NULL;
    if(curback){
        printf("background processes [%d] ", curback);
		for(int i = 0; i<curback;i++) {
				printf("%d ",backproc[i]);
			}
		printf("\n");
        hunt();
    }
}

//Финальная очистка процессов
void final_clean(){
	cleanprocess();
	free(backproc);
	curback = 0;
	backproc = NULL;
}

//Проверка статуса завершения команды
int checkend(int status){
    return WIFEXITED(status) && !WEXITSTATUS(status);
}

//подключение файла вывода
int outfile(cmd *command){
    int f;
    if(command->append){
        f = open(command->outfile, O_WRONLY | O_APPEND | O_CREAT, 0666);
        if(f == -1){
            printf("error open file %s\n", command->outfile);
            return 1;
        }
    }
    else{
        f = open(command->outfile, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if(f == -1){
            printf("error open file %s\n", command->outfile);
            return 1;
        }
    }
    dup2(f, 1);
    close(f);
    return 0;
}

//Подключение файла ввода
int infile(cmd *command){
    int f;
    struct stat buf;
    stat(command->infile,&buf);
    f = open(command->infile,O_RDONLY,buf.st_mode);
    if(f == -1){
        printf("error open file %s\n", command->infile);
        return 1;
    }
    dup2(f,0);
    close(f);
    return 0;
}

//Добавление процессов в таблицу процессов
void addprocess(int pid, cmd* command){
    if(command->backgrnd){
        curback++;
        backproc = (int*)realloc(backproc,sizeof(int)*curback);
        backproc[curback-1] = pid;
    } else {
        curactive++;
        activeproc = (int*)realloc(activeproc,curactive*sizeof(int));
        activeproc[curactive-1] = pid;
    }
}

//Конвеер команд
cmd* conveer(cmd *command){
    int fd[2], in=-1, out=-1, next_in=-1, first=1, pid;
    pipe(fd); next_in = fd[0]; out = fd[1];
    if(!(pid=fork())){
        if(command->infile != NULL)
            if(infile(command)){
                command->error = 1;
                return command;
            }
        if(command->outfile != NULL)
            if(outfile(command)){
                command->error = 1;
                return command;
            }
        if(command->backgrnd){
            signal(SIGINT,SIG_IGN);
            close(0);
        }
        close(next_in);
        if(command->outfile == NULL) dup2(out,1);
        close(out);
        execvp(command->argv[0],command->argv);
        command->error = 1;
        return command;
    }
    addprocess(pid,command);
    in = next_in;
    command = command->pipe;
    while(command->pipe){
        close(out);
        pipe(fd);
        out = fd[1];
        next_in = fd[0];
        if(!(pid=fork())){
            if(command->backgrnd) signal(SIGINT,SIG_IGN);
            if(command->infile != NULL)
                if(infile(command)){
                    command->error = 1;
                    return command;
                }
            if(command->outfile != NULL)
                if(outfile(command)){
                    command->error = 1;
                    return command;
                }
            close(next_in);
            if(command->infile == NULL) dup2(in,0);
            close(in);
            if(command->outfile == NULL) dup2(out, 1);
            close(out);
            execvp(command->argv[0], command->argv);
            printf("error execution command %s\n", command->argv[0]);
            command->error = 1;
            return command;
        }
        close(in);
        addprocess(pid,command);
        in = next_in;
        command = command->pipe;
    }
    close(out);
    if (!(pid=fork())){
        if(command->backgrnd) signal(SIGINT,SIG_IGN);
        if(command->infile != NULL)
            if(infile(command)){
                command->error = 1;
                return command;
            }
        if(command->outfile != NULL)
            if(outfile(command)){
                command->error = 1;
                return command;
            }
        if(command->infile == NULL) dup2(in,0);
        close(in);
        execvp(command->argv[0],command->argv);
        printf("error execution command %s\n", command->argv[0]);
        command->error = 1;
        return command;
    }
    addprocess(pid,command);
    close(in);
    return command->next;
}

//Последовательное выполнение команд
cmd* cmdnext(cmd *command){
    int logic=0, status, good = 1, pid;
    if(command->flag == 3) logic = 3;
    if(command->flag == 4) logic = 4;
    while(command->next){
        if(!(pid = fork()) && good){
            if(command->backgrnd){
                signal(SIGINT,SIG_IGN);
                close(0);
            }
            if(command->infile != NULL)
                if(infile(command)){
                    command->error = 1;
                    return command;
                }
            if(command->outfile != NULL)
                if(outfile(command)){
                    command->error = 1;
                    return command;
                }
            execvp(command->argv[0],command->argv); 
            printf("error execution command %s\n", command->argv[0]);
            command->error = 1;
            return command;
        }
        addprocess(pid,command);
        if(command->backgrnd == 0){
            waitpid(pid,&status,0);
            if(logic != 0) good = (logic==3)?checkend(status):!checkend(status);
        }
        command = command->next;
    }
    if(!(pid=fork()) && good){
        if(command->infile != NULL)
            if(infile(command)){
                command->error = 1;
                return command;
            }
        else if(command->backgrnd){
            signal(SIGINT,SIG_IGN);
            close(0);
        }
        if(command->outfile != NULL)
            if(outfile(command)){
                command->error = 1;
                return command;
            }
        execvp(command->argv[0],command->argv);
        fprintf(stderr,"error execution command %s\n", command->argv[0]);
        command->error = 1;
        return command;
    }
    addprocess(pid,command);
    if(command->backgrnd == 0){
        waitpid(pid,&status,0);
        if(logic != 0) good = (logic==3)?checkend(status):!checkend(status);
    }
    return command->pipe;
}

//Команда cd
void cmdcd(int argc, char ** argv){
	char *s;
	if (argc == 0){
		s = getenv("HOME");
		if (s == NULL) printf("wrong directory\n");
        else  chdir(s);
	} else if (argc > 1) printf("too much arguments\n");
    else if (chdir(argv[1])) printf("wrong directory\n");
}

//Подстановки
void substitution(cmd *command, char **mass){
    int j = 0, len, i;
    struct passwd *temp;
    cmd *p = command;
    while(p){
        j = 0;
        if(p -> argv){
            while(p -> argv[j]){
                if(!strcmp(p -> argv[j],"$HOME")){
                    len = strlen(getenv("HOME"));
                    i = 0;
                    while(strcmp(mass[i],"$HOME")) i++;
                    free(mass[i]);
                    mass[i] = malloc(len + 1);
                    mass[i][len] = '\0';
                    strcpy(mass[i], getenv("HOME"));
                    p->argv[j] = mass[i];
                } else if(!strcmp(p -> argv[j],"$SHELL")){
                    len = strlen(getenv("SHELL"));
                    i = 0;
                    while(strcmp(mass[i],"$SHELL")) i++;
                    free(mass[i]);
                    mass[i] = malloc(len + 1);
                    mass[i][len] = '\0';
                    strcpy(mass[i], getenv("SHELL"));
                    p->argv[j] = mass[i];
                } else if(!strcmp(p -> argv[j],"$USER")){
                    temp = getpwuid(geteuid());
                    i=0;
                    while(strcmp(mass[i],"$USER")) i++;
                    free(mass[i]);
                    mass[i] = malloc(strlen(temp->pw_name)+1);
                    strcpy(mass[i], temp -> pw_name);
                    p -> argv[j] = mass[i];
                } else if(!strcmp(p -> argv[j],"$EUID")){
                    int len, temp, uid = (int) geteuid();
                    temp = uid;
                    for (len = 0; temp > 9; len++) 
                        temp = temp / 10;
                    len++;
                    i=0;
                    while(strcmp(mass[i],"$EUID")) i++;
                    free(mass[i]);
                    mass[i] = malloc(len + 1);
                    mass[i][len] = '\0';
                    len--;
                    for (; uid != 0; len--) {
                        temp = uid % 10 + '0';
                        mass[i][len] = temp ;
                        uid = uid / 10;
                    }
                    p->argv[j] = mass[i];
                }
                j++;
            }
        }
        if(p->pipe) p = p -> pipe;
        else p = p -> next;
    }
}

//Выполнение команд
int process(cmd *command){
    if (command == NULL){
        cleanprocess();
        return 0;
    }
    if (command->argv && !strcmp(command->argv[0], "exit")) return 1;
    if (command->argv && !strcmp(command->argv[0], "cd")){
        int i=0;
        while(command->argv[i]) i++;
        cmdcd(i-1, command->argv);
        return 0;
    }
    while(command && command->argv && command->error != 1){
        if(command->pipe != NULL) command = conveer(command);
        else command = cmdnext(command);
    }
    cleanprocess();
    if (command && command->error == 1) return 1;
    return 0;
}

//Заносим флаг внешнего процесса в конвеер
void back_pipe(cmd *curr){
    if(curr == NULL) return;
    if(curr -> pipe){
        back_pipe(curr -> pipe);
        curr -> backgrnd = curr -> pipe -> backgrnd;
    }
}

//Устанавливаем флаг внешнего процесса
void set_back(cmd *list){
    if(list == NULL) return;
    cmd *curr = list;
    while(curr -> pipe || curr -> next){
        back_pipe(curr);
        while(curr -> pipe)
            curr = curr -> pipe;
        if(curr -> next) curr = curr -> next;
    }
}

//Очищаем структуры команд
void clearcmd(cmd *command){
    cmd *temp;
    while(command){
        temp = command;
        if(command->next) command = command->next;
        else command = command->pipe;
        free(temp->argv);
        temp->argv = NULL;
        temp->outfile = NULL;
        temp->infile = NULL;
        free(temp);
    }
}

//Формируем структуры команд
cmd* analysis(char **lex, int curlen){
    int i = 0, last = 0, flag, j;
    int curcom=0, sizearg=0;
    char **com = NULL;
    cmd *command=NULL, *temp, *lastcom=NULL;
    command = temp = creatcom();
    while(i<curlen-1){
        switch (flag=check(lex[i])){
        case 0: // arguments
            while(i<curlen-1 && !(flag=check(lex[i]))) i++;
            com = (char**)calloc(i-last+1,sizeof(cmd));
            for(j = last; j<i; j++) com[j-last] = lex[j];
            temp->argv = com;
            if(flag == 3 || flag == 4) temp->flag = flag;
            lastcom = temp;
            break;
        case 1: // ;
        case 3:
        case 4:
            last = ++i;
            if(i<curlen-1) temp = temp->next = creatcom();
            break;
        case 2: // &
            last = ++i;
            temp->backgrnd = 1;
            if(i<curlen-1) temp = temp->next = creatcom();
            break;
        case 7: // >>
            temp->append = 1;
        case 5: // >
            i++;
            if(i<curlen-1) temp->outfile = lex[i];
            else printf("syntax error near unexpected token `newline'\n");
            i++;
            break;
        case 6: // <
            i++;
            if(i<curlen-1) temp->infile = lex[i];
            else printf("syntax error near unexpected token `newline'\n");
            i++;
            break;
        case 8: // |
            last = ++i;
            if(i<curlen-1) temp = temp->pipe = creatcom();
            break;
        }
    }
    return command;
}