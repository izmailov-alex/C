#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 16
#define N 10
extern void null_list();
extern void clearlist();
extern void termlist();
extern void printlist();
extern void sortlist();

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
    if ((k = fscanf(stdin, "%15[^\n]s", inp)) == EOF) {free(inp); return 1;}
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
void find_maxlen_and_minlen(char **mass, int sizelist){
    int i, j, maxlen=0, minlen=0, len, curmax, curmin;
    if (mass != NULL && sizelist > 1){
        for(i=0; i<sizelist-1; i++){
            j = 0;
            len = 0;
            while(mass[i][j] != '\0'){
                len++;
                j++;
            }
            if(len > maxlen){
                curmax = i;
                maxlen = len;
            }
            if (len < minlen || minlen == 0){
                curmin = i;
                minlen = len;
            }
        }
        printf("maxstring:%s\nmaxlen = %i\n", mass[curmax], maxlen);
        printf("minstring:%s\nminlex = %i\n", mass[curmin], minlen);
    }
}
void maxspec(char **mass, int sizelist){
    int i, j, k, l, max=0;
    char lit[2];
    struct dictionary {
        char* ch;
        int num;
    } spec[N] = {
        {"|", 0},
        {"||", 0},
        {"&", 0},
        {"&&", 0},
        {">", 0},
        {">>", 0},
        {"<", 0},
        {";", 0},
        {")", 0},
        {"(", 0},
    };
    if (mass != NULL && sizelist > 1){
        for(i=0; i<sizelist-1; i++){
            j=0;
            while (mass[i][j] != '\0'){
                if (!check1(mass[i][j])){
                    lit[0] = mass[i][j];
                    if(check2(mass[i][j]) && mass[i][j+1]==mass[i][j]){
                        lit[1] = mass[i][j+1];
                        for(l = 0; l<N; l++){
                            k = 0;
                            while(lit[k] == spec[l].ch[k] && lit[k] && spec[l].ch[k]) k++;
                            if (lit[k]-spec[l].ch[k] == 0) {
                                spec[l].num++;
                                break;
                            }
                        }
                        j = j + 2;
                    } else {
                        lit[1] = '\0';
                        for(l = 0; l<N; l++){
                            k = 0;
                            while(lit[k] == spec[l].ch[k] && lit[k] && spec[l].ch[k]) k++;
                            if (lit[k]-spec[l].ch[k] == 0) {
                                spec[l].num++;
                                break;
                            }
                        }
                        j++;
                    }
                } else j++;
            }
        }
        for(i=0; i<N; i++){
            if (max < spec[i].num){
                max = spec[i].num;
                j = i;
            }
        }
        if(max == 0)printf("Специальных символов в строчке нет\n");
        else printf("%s : %i\n", spec[j].ch, spec[j].num);
    }
}
void deland2(char ***mass, int *sizelist){
    //&&aa&&bb;;cc <-> &&aabb;;cc
    int i, j, k1=-1, k2=-1, k3=-1, count=0;
    char *buff = NULL;
    if (*mass != NULL && *sizelist > 1){
        i = 0;
        while(i<*sizelist-1){
            if((*mass)[i][0] == '&' && (*mass)[i][1] == '&'){
                k1 = i;
            }
            if((*mass)[i][0] == ';' && i+1 < (*sizelist)-1 && (*mass)[i+1][0] == ';' && k1 != -1){
                free((*mass)[k1]);
                count++;
                (*sizelist)--;
                k3 = k1 + 1;
                if (k1 && check1((*mass)[k1-1][0]) && check1((*mass)[k1+1][0])){
                    buff = (char*)malloc((strlen((*mass)[k1-1])+strlen((*mass)[k1+1])+1)*sizeof(char));
                    j = 0;
                    while((*mass)[k1-1][j]) {
                        buff[j] = (*mass)[k1-1][j];
                        j++;
                    }
                    k2 = j;
                    j = 0;
                    while((*mass)[k1+1][j]){
                        buff[k2] = (*mass)[k1+1][j];
                        j++;
                        k2++;
                    }
                    buff[k2] = '\0';
                    free((*mass)[k1-1]);
                    free((*mass)[k1+1]);
                    k3 = k1 + 2;
                    (*mass)[k1-1] = buff;
                    (*sizelist)--; 
                }
                j = k1;
                while((*mass)[k3]){
                    (*mass)[j] = (*mass)[k3];
                    j++;
                    k3++;
                }
                (*mass) = realloc(*mass, ((*sizelist)+1)*sizeof((*mass)[0]));
                (*mass)[*sizelist-1] = NULL;
            }
            i++;
        }
    }
    printf("count = %i\n", count);
}

int main() {
    typedef enum {Start, Word, SpecWord, Great1, Great2, NewLine, Stop} vertex;
    vertex state = Start;
    char *str;
    int sizelist, curlist, sizebuf, curbuf, c, dop, curelem = 0;
    char *buf;
    char **mass;
    str = (char*)malloc(15*sizeof(char)+1);
    if (upd(&curelem, str)) exit(0);
    c = getsym(&curelem, str);
    null_list(mass, &sizelist, &curlist);
    mass = (char**)malloc((sizelist+=SIZE)*sizeof(*mass));
    while(1){
        switch (state) {
        case Start:
            if (c==' ' || c=='\t') c = getsym(&curelem, str);
            else if (c == '\0') {
                state = NewLine;                
                termlist(&mass, &sizelist, &curlist);
                printlist(&mass, &sizelist);
                deland2(&mass, &sizelist);
                printlist(&mass,&sizelist);
                sortlist(&mass, &sizelist);
                printlist(&mass, &sizelist);
                find_maxlen_and_minlen(mass,sizelist);
                maxspec(mass, sizelist);
                curelem = SIZE;
                c = getsym(&curelem, str);
            }
            else if (c == EOF) {
                state = Stop;
                termlist(&mass, &sizelist, &curlist);
                printlist(&mass, &sizelist);
                deland2(&mass, &sizelist);
                printlist(&mass,&sizelist);
                sortlist(&mass, &sizelist);
                printlist(&mass, &sizelist);
                find_maxlen_and_minlen(mass, sizelist);
                maxspec(mass, sizelist);
                clearlist(mass, &sizelist, &curlist);
            }
            else {
                null_buf(buf, &sizebuf, &curbuf);
                buf = malloc(sizebuf+=SIZE);
                addsym(c, &buf, &sizebuf, &curbuf);
                state = check1(c)? Word:(check2(c)?Great1:SpecWord);
            }
            break;
        
        case Word:
            c = getsym(&curelem, str);
            if (symset(c)){
                if (check1(c)){
                    addsym(c, &buf, &sizebuf, &curbuf);
                } else {
                    addword(&buf, &sizebuf, &curbuf, &mass, &sizelist, &curlist);
                    state = Start;
                }
            } else {
                addword(&buf, &sizebuf, &curbuf, &mass, &sizelist, &curlist);
                state = Start;
            }
            break;

        case SpecWord:
            addword(&buf, &sizebuf, &curbuf, &mass, &sizelist, &curlist);
            c = getsym(&curelem, str);
            state = Start;
            break;
        
        case Great1:
            dop = c;
            c = getsym(&curelem, str);
            if (c == dop){
                addsym(c, &buf, &sizebuf, &curbuf);
                c = getsym(&curelem, str);
                state = Great2;
            } else {
                addword(&buf, &sizebuf, &curbuf, &mass, &sizelist, &curlist);
                state = check2(c)?Great1:Start;
            }
            break;
        case Great2:
            addword(&buf, &sizebuf, &curbuf, &mass, &sizelist, &curlist);
            state = Start;
            break;
        
        case NewLine:
            clearlist(mass, &sizelist, &curlist);
            mass = (char**)malloc((sizelist+=SIZE)*sizeof(*mass));
            state = Start;
            break;
        case Stop:
            exit(0);
            break;
        }
    }
    return 0;
}
