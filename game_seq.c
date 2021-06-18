#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
            /*  game of life    */


 #define  ml 1000       // nombre des lignes
 #define mc 1000        // nombre des colonnes

//etat d'un cellule
enum{
    dead, alive
};

//fonction pour afficher la matrice
void print_matrix(char *m, int l, int c){
    for(int i=0; i<c+2; i++){
        printf("--");
    }
    printf("\n");
    for(int i=0; i<l; i++){
        printf("| ");
        for(int j=0; j<c; j++){
            if(*(m + i*c+ j)){
                printf("##");
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

// fonction de la mise à jour d'un cellule
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

// focntion de mise à jour de la matrice
void update(char *m, char *bis, int l, int c){
    for(int i=0; i<l; i++){
        for(int j=0; j<c; j++){
            *(bis + i * c +j) = update_cell(m,i,j,l,c);
        }
    }
    for (int i = 0; i < l; i++)
    {
        for(int j=0; j<c; j++){
            *(m + i * c +j) = *(bis + i * c +j);
        }
    }
    
}

//  fonction de initialisation de  matrice avec la configuration
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
    int nbr_gen;
    printf("BIENVENU AU JEU DE LA VIE\n");
    printf("veuilllez entres le nombre des generations : ");
    scanf("%d",&nbr_gen);
    char matrix[ml][mc]= {dead};
    char bis[ml][mc] = {dead};

    //  la matrice de configuration
    char canon[9][36] = {
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
          {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
          {1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
          {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        };

    // initialiser la matrice matrix avec la configuration canon
    set_matrix(&matrix[0][0], ml, mc, &canon[0][0], 9, 36, 10, 10);

    int t1 = clock();
    for(int i=0; i<nbr_gen;i++){
        update(&matrix[0][0], &bis[0][0], ml, mc);
    }
    int t2 = clock();
    //calclue de temps d'éxecution
    float temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    printf("temps = %f\n", temps);
    return 0;
}