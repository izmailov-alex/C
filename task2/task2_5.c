#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node *link;
typedef char elemtype;
typedef struct Node {
    elemtype *elem;
    link next;
    } node;
typedef link list;

void add_elem(list *ls, char *s){
    list cur;
    if (*ls == NULL) {
        *ls = (list)malloc(sizeof(node));
        (*ls)->elem = s;
        (*ls)->next = NULL;
    } else {
        cur = (list)malloc(sizeof(node));
        cur->elem = s;
        cur->next = *ls;
        *ls = cur;
    }
}

void rec_print(list p){
    if (p==NULL) printf("\n");
    else {
        rec_print(p->next);
        printf("%s ", p->elem);
    }
}

void delete(list spis, char *last){
    list cur1, cur2;
    if (spis != NULL && spis->next != NULL){
        cur1 = spis;
        cur2 = spis->next;
        while(cur2 != NULL){
            if (strcmp(cur2->elem, last) == 0){
                cur1->next = cur2->next;
                free(cur2);
                cur2 = cur1->next;
            } else {
                cur2=cur2->next;
                cur1=cur1->next;
            }
        }
    }
}
void reverse(list *sp) {
    list t=*sp;
    list cop;
    add_elem(sp, t->elem);
    (*sp)->next = NULL;
    cop = t->next;
    free(t);
    t = cop;
    while (t != NULL){
      add_elem(sp,t->elem);
      cop = t->next; 
      free(t);
      t = cop;
    }
    
}
void swap(list *sp){
    list cur1, cur2, temp, last;
    cur1 = *sp;
    cur2 = (*sp)->next;
    *sp = cur2;
    last = NULL;
    while(cur2 != NULL){
        temp = cur2->next;
        cur2->next = cur1;
        cur1->next = temp; 
        if (last != NULL) last->next = cur2;
        last = cur1;
        cur1 = cur1->next;
        (cur1==NULL)?(cur2=NULL):(cur2=cur1->next);
    }
}
void print(list sp){
    while (sp != NULL){ 
        printf("%s ", sp->elem);
        sp = sp->next;
    }
}
void delete_chet(list *sp){
    list cur1, cur2, temp;
    if (*sp != NULL && (*sp)->next!=NULL){
        cur1 = *sp;
        while(cur1 != NULL && cur1->next != NULL){
            cur2 = cur1->next;
            temp = cur2->next;
            free(cur2);
            cur1->next = temp;
            cur1 = cur1->next;
        }
    }
    
}
int main(){
    list spis=NULL;
    char *str, *last;
    size_t size = 10;
    while(scanf("%s", str = (char*)malloc(size*sizeof(char))) != EOF){
        add_elem(&spis, str);
        last = str;
    }
   delete(spis, last);
   rec_print(spis);
   printf("\n");
   reverse(&spis);
  //swap(&spis);
   print(spis);
   printf("\n");
   delete_chet(&spis);
   print(spis);
   printf("\n");
   return 0;
}
