#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 16
#define N 10

void null_list();
void clearlist();
void termlist();
void printlist();
cmd* analysis();
void final_clean();
void inv();
void null_buf();
void addsym();
int check1();
int check2();
int symset();
void addword();
int upd();
int getsym();


int construct() {
    typedef enum {Start, Word, SpecWord, Great1, Great2, NewLine, Quotes, Stop} vertex;
    vertex state = Start;
    char *str;
    int sizelist, curlist, sizebuf, curbuf, c, dop, curelem = 0, err;
    char *buf;
    char **mass;
    cmd *command;
    inv();
    str = (char*)malloc(15*sizeof(char)+1);
    if (upd(&curelem, str)) exit(0);
    c = getsym(&curelem, str);
    null_list(mass, &sizelist, &curlist);
    mass = (char**)calloc((sizelist+=SIZE),sizeof(*mass));
    while(1){
        switch (state) {
        case Start:
            if (c==' ' || c=='\t') c = getsym(&curelem, str);
            else if (c == '\0') {
                state = NewLine;                
                termlist(&mass, &sizelist, &curlist);
                command = analysis(mass, sizelist);
                set_back(command);
                substitution(command, mass);
                err = process(command);
                clearcmd(command);
                curelem = SIZE;
            }
            else if (c == EOF) {
                state = Stop;
                termlist(&mass, &sizelist, &curlist);
                command = analysis(mass, sizelist);
                set_back(command);
                substitution(command, mass);
                process(command);
                clearcmd(command);
                final_clean();
                printf("\n");
                clearlist(mass, &sizelist, &curlist);
            }
            else {
                null_buf(buf, &sizebuf, &curbuf);
                buf = malloc(sizebuf+=SIZE);
                if(c=='"') state = Quotes;
                else{
                    addsym(c, &buf, &sizebuf, &curbuf);
                    state = check1(c)? Word:(check2(c)?Great1:SpecWord);
                }
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
            if(err == 1){
                free(str);
                exit(0);
            }
            inv();
            c = getsym(&curelem, str);
            mass = (char**)calloc((sizelist+=SIZE),sizeof(*mass));
            state = Start;
            break;
        case Quotes:
            c = getsym(&curelem, str);
            if(c=='"'){
                addword(&buf, &sizebuf, &curbuf, &mass, &sizelist, &curlist);
                c = getsym(&curelem, str);
                state = Start;
            } else if(c == EOF || c =='\0'){
                free(buf);
                c = getsym(&curelem, str);
                curelem = SIZE;
                fprintf(stderr,"lost second symbol \"\n");
                state = NewLine;
            } else{
                addsym(c, &buf, &sizebuf, &curbuf);
            }
            break;
        case Stop:
            exit(0);
            break;
        }
    }
    return 0;
}
