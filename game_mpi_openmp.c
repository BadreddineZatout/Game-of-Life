#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <time.h> 

    /*  game of life    */

 #define  ml 1000   // nombre des lignes
 #define mc 1000    // nombre des colonnes

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
char update_cell(char m[ml][mc], int ipos, int jpos){
    int nb =0;
    int i, j;
    for (i = ipos-1; i <= ipos+1; i++)
    {
        if(i>=0 && i<ml){
            for(j=jpos-1; j<=jpos+1; j++){
                if(j>=0 && j<mc){
                    if(i!=ipos || j!=jpos){
                        if((m[i][j])==alive){
                            nb++;
                        }
                    }
                }
            }
        }
    }
    if(m[ipos][jpos]==alive){
        if(nb==2 || nb==3) return alive;
        else return dead;
    }else{
        if(nb==3) return alive;
        else return dead;
    }
}

// focntion de mise à jour de la matrice
void update(char m[ml][mc], char bis[ml][mc], int l, int c){
    int rank , code, size ;
    MPI_Status status;
    const int root = 0;
    code = MPI_Comm_size(MPI_COMM_WORLD,&size);
    code = MPI_Comm_rank(MPI_COMM_WORLD ,&rank);
    char local[ml][mc];
    //Calculer le nombre des ligne à donnés pour chaque processus
    int cols_for_each_proc = c / size;
    if(c%size != 0) {cols_for_each_proc++;}
    for(int i=0;i<l;i++){
        //on devise les colonnes de la matrice sur les processus
        MPI_Scatter(m[i], cols_for_each_proc ,MPI_CHAR ,local[i] , cols_for_each_proc ,MPI_CHAR,root ,MPI_COMM_WORLD);
    }
    //Synchronization des processus
    MPI_Barrier(MPI_COMM_WORLD);
    //Définition de nombre de threads
    omp_set_num_threads(4);
    //Définition de la région parallèle pour la mise à jour de matrice
    #pragma omp parallel for
    for(int i=0; i<l; i++){
        for(int j=0; j<cols_for_each_proc && j*rank<c; j++){
            bis[i][j+rank*cols_for_each_proc] = update_cell(m,i,j+rank*cols_for_each_proc);
            local[i][j]=bis[i][j+rank*cols_for_each_proc];
        }
    }
    //Synchronisation des threads
    #pragma omp barrier
    //Synchronization des processus
    MPI_Barrier(MPI_COMM_WORLD);
    for(int i=0;i<l;i++){
        //Rassembler les resultats de chaque processus dans la matrice
        MPI_Allgather( local[i] ,  cols_for_each_proc ,  MPI_CHAR , m[i] , cols_for_each_proc , MPI_CHAR , MPI_COMM_WORLD ); 
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

int main(int argc,char **argv){
    int nbr_gen = atoi(argv[1]); 
    // Example of execution: [# mpirun -np 2 game 3 ] , 3 is the number of generations
    printf("BIENVENU AU JEU DE LA VIE\n");
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
    set_matrix(&matrix[0][0], ml, mc, &canon[0][0], 9, 36, 0, 0);
    // MPI
    int code, i;    
    code = MPI_Init( NULL,NULL);
    int t1 = clock();
    for(int i=0; i<nbr_gen;i++){
        update(matrix, bis, ml, mc);
    }
    int t2 = clock();
    //calclue de temps d'éxecution
    float temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    printf("temps = %f\n", temps);
    MPI_Finalize();    
    return 0;

}