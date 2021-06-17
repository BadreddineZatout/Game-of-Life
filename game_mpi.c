#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h> 
/*  game of lide    */


 #define  ml 80
 #define mc 80

enum{
    dead, alive
};

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

void update(char m[ml][mc], char bis[ml][mc], int l, int c){
    int rank , code, size ;
    MPI_Status status;
    const int root = 0;
    code = MPI_Comm_size(MPI_COMM_WORLD,&size);
    code = MPI_Comm_rank(MPI_COMM_WORLD ,&rank);
    printf("proc %d / %d \n",rank,size);
    char local[ml][mc];
    // Calculate how much lines to give to every processor :
    int cols_for_each_proc = c / size;
    int col_terminal = l-(cols_for_each_proc*size-1);
    if(c%size != 0) cols_for_each_proc++;
    printf("Lines for each proc : %d\n",cols_for_each_proc);
    for(int i=0;i<l;i++){
        MPI_Scatter(m[i], cols_for_each_proc ,MPI_CHAR ,local[i] , cols_for_each_proc ,MPI_CHAR,root ,MPI_COMM_WORLD);
    }
    for(int i=0; i<l; i++){
        for(int j=0; j<cols_for_each_proc && j*rank<c; j++){
            bis[i][j+rank*cols_for_each_proc] = update_cell(&m[0][0],i,j+rank*cols_for_each_proc,l,c);
            local[i][j]=bis[i][j+rank*cols_for_each_proc];
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for(int i=0;i<l;i++){
        MPI_Gather( local[i] ,cols_for_each_proc , MPI_CHAR ,m[i] ,cols_for_each_proc ,MPI_CHAR ,root ,MPI_COMM_WORLD);
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

int main(int argc,char **argv){
    // int l, c;
    int nbr_gen = atoi(argv[1]); 
    // Example of execution: [# mpirun -np 2 game 3 ] , 3 is the number of generations
    printf("BIENVENU AU JEU DE LA VIE\n");
    // MPI
    int rank , i, code, size ;
    // MPI_Init( int* argc , char*** argv);
    code = MPI_Init( NULL,NULL);
    MPI_Status status;
    code = MPI_Comm_size(MPI_COMM_WORLD,&size);
    code = MPI_Comm_rank(MPI_COMM_WORLD ,&rank);
    // printf("veuilllez entres les dimensions : \n");
    // printf("Nombre des lignes : ");
    // scanf("%d",&l);
    // printf("\nNombre des colonnes : ");
    // scanf("%d",&c);
    if(rank==0){
        printf("Nombre de generation : %d\n",nbr_gen);
    }
    char matrix[ml][mc]= {dead};
    char bis[ml][mc] = {dead};
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
    set_matrix(&matrix[0][0], ml, mc, &canon[0][0], 9, 36, 10, 10);
    int t1 = clock();
    for(int i=0; i<nbr_gen;i++){
        //system("cls");
        update(matrix, bis, ml, mc);
        if(rank == 0){
            print_matrix(&matrix[0][0], ml, mc);
        }
        //Sleep(10);
    }
    int t2 = clock();
    float temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    printf("temps = %f\n", temps);
    MPI_Finalize();

    return 0;
}