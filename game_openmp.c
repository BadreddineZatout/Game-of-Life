#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
/*  game of lide    */


#define  maxl 2000
#define maxc 2000

enum{
    dead, alive
};

void print_matrix(char *m, int l, int c){
    for(int i=0; i<c+2; i++){
        printf("--");
    }
    printf("\n");
    for(int i=0; i<maxl; i++){
        printf("| ");
        for(int j=0; j<c; j++){
            if(*(m + i*c+ j)){
                printf("\033[105m \033[0m");
            }else{
                printf("  ");
            }
        }
        printf(" |\n");
    }
    for(int i=0; i<c+2; i++){
        printf("--");
    }
}

char update_cell(char *m, int ipos, int jpos, int l, int c){
    int nb =0;
    int i, j;
    for (i = ipos-1; i <= ipos+1; i++)
    {
        if(i>=0 && i<l){
            for(j=jpos-1; j<=jpos+1; j++){
                if(j>=0 && j<c){
                    if(i!=ipos || j!=jpos){
                        if(*(m + i * c + j)==alive){
                            nb++;
                        }
                    }
                }
            }
        }
    }
    if(*(m + ipos * c + jpos)==alive){
        if(nb==2 || nb==3) return alive;
        else return dead;
    }else{
        if(nb==3) return alive;
        else return dead;
    }
}

void updateV1(char *m, char *bis, int l, int c){
    omp_set_num_threads(1);
    #pragma omp parallel for
    for(int i=0; i<l; i++){
        for(int j=0; j<c; j++){
            *(bis + i * c +j) = update_cell(m,i,j,l,c);
        }
    }
    #pragma omp barrier
    #pragma omp parallel for
    for (int i = 0; i < l; i++)
    {
        for(int j=0; j<c; j++){
            *(m + i * c +j) = *(bis + i * c +j);
        }
    }
    
}

void set_matrix(char *m, int l, int c,char *s, int sl, int sc, int x, int y){
    for (int i = y; i < y+sl; i++)
    {
        for (int j = x; j < x+sc; j++)
        {
            *(m + i * c + j) = *(s + (i-y) * sc + (j-x));
        }
        
    }
    
}

int main(void){
    char matrix[maxl][maxc] = {dead};
    char bis[maxl][maxc]= {dead};
    char canon[3][3] = {
          {0,0,0},
          {1,1,1},
          {0,0,0},
        };
    set_matrix(&matrix[0][0], maxl, maxc, &canon[0][0], 3, 3, 10, 10);
    int i=0;
    double t1= omp_get_wtime();
    while(i<=200)
    {
        updateV1(&matrix[0][0], &bis[0][0], maxl, maxc);
        i++;
    }
    printf("le temps %lf",omp_get_wtime()-t1);
    return 0;
}