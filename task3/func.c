#include <stdio.h>
#include <stdlib.h>

//инициализация списка
void null_list(char **mass, int *sizelist, int *curlist){
    mass = NULL;
    *sizelist = 0;
    *curlist = 0;
}
//очищение списка
void clearlist(char **mass, int *sizelist, int *curlist){
    int i;
    *curlist = 0;
    *sizelist = 0;
    if (mass != NULL){
        for(i=0; mass[i] != NULL; i++){ 
            free(mass[i]);
        }
        free(mass);
        mass = NULL;
    }
}
//корректировка размеров списка
void termlist(char ***mass, int *sizelist, int *curlist){
    if (*mass != NULL){
        if (*curlist>(*sizelist)-1) *mass = realloc(*mass, ((*sizelist)+1)*sizeof(**mass));
        (*mass)[*curlist] = NULL;
        *sizelist = (*curlist)+1;
        *mass = realloc(*mass, ((*sizelist)*sizeof(**mass)));
    }
}
//печать списка
void printlist(char ***mass, int *sizelist){
    int i;
    if (*mass != NULL){
        for(i = 0; i<(*sizelist)-1; i++){
            printf("%s " , (*mass)[i]);
        }
    }
    printf("\n");
}
//сортировка списка
void sortlist(char ***mass, int *sizelist) {
    int i, j, c, flag;
    char *temp;
    if (*mass != NULL){
        for(j = 1; j<(*sizelist)-1; j++){
            for(i = 0; i<(*sizelist)-1-j; i++){
                c = 0;
                flag = 1;
                while ((*mass)[i][c] != '\0' && (*mass)[i+1][c] != '\0'){
                    if ((*mass)[i][c] > (*mass)[i+1][c]) {
                        temp = (*mass)[i];
                        (*mass)[i] = (*mass)[i+1];
                        (*mass)[i+1] = temp;
                        flag = 0;
                        break;
                    } else if ((*mass)[i][c] < (*mass)[i+1][c]){
                        flag = 0;
                        break;
                    }
                    c++;
                }
                if ((*mass)[i][c] != '\0' && (*mass)[i+1][c]=='\0' && flag){
                    temp = (*mass)[i];
                    (*mass)[i] = (*mass)[i+1];
                    (*mass)[i+1] = temp;  
                }
            }
        }
    }
}