#include <stdio.h>
#include <stdlib.h>

typedef struct tNode *tree;
typedef int elemtype;
typedef struct tNode {
    elemtype elem;
    tree left;
    tree right;
} tnode;

// Добавляет элемент в дерево 
void add_elem(tree *p, int a){
    if (*p == NULL){
        printf("head->%i\n", a);
        *p = (tnode*)malloc(sizeof(tnode));
        (*p)->elem = a;
        (*p)->left = NULL;
        (*p)->right = NULL;
    } else if ((*p)->elem < a){
        printf("right->");
        add_elem(&((*p)->right), a);
    } else {
        printf("left->");
        add_elem(&((*p)->left), a);
    }
}

// Проверяет наличие элемента в дереве
int check(tree *p, int a){
    if (*p == NULL) return 0;
    else if ((*p)->elem < a) return check(&((*p)->right), a);
    else if ((*p)->elem > a) return check(&((*p)->left), a);
    else return 1;
}

/* Удаляет элемент из двоичного дерева */
void delete_elem(tree *p, int a){ 
    if (*p == NULL) ;
    else if ((*p)->elem < a) delete_elem(&((*p)->right), a);
    else if ((*p)->elem > a) delete_elem(&((*p)->left), a);
    else if ((*p)->elem == a) {
        if(((*p)->right == NULL) && ((*p)->left == NULL)) {
            free(*p);
            *p = NULL;
        } else if (((*p)->right != NULL) && ((*p)->left == NULL)) {
            tree p2 = (*p)->right;
            free(*p);
            *p = p2;
        } else if (((*p)->left != NULL) && ((*p)->right == NULL)) {
            tree p2 = (*p)->left;
            free(*p);
            *p = p2;
        } else {
            // случай наличия двух детей у удаляемого элемента
            tree step = (*p)->right;
            while (step->left != NULL) {
               step = step->left;
            }
            tree l = step->right;
            (*p)->elem = step->elem;
            free(step);
            step = l;
        }
    }
}

int main(){
    int ch;
    char el;
    tree t=NULL;
    while(scanf("%c", &el) != EOF){
        if (el == '\n') scanf("%c",&el);
        scanf("%i", &ch);
        switch (el) {
            case '+': {
                if (!check(&t, ch)) add_elem(&t, ch);
                break;
            }
            case '-':{
                if (check(&t, ch)) delete_elem(&t, ch);
                break;
            }
            case '?':{
                printf("%i ", ch);
                if (!check(&t,ch)) printf("No\n");
                else printf("Yes\n");
                break;
            }
        }
    }
    return 0;
}
