#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

/*  game of life    */


#define  maxl 1000      // nombre des lignes
#define maxc 1000       // nombre des colonnes

pthread_cond_t cv =PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
int nb_generations =200;        //nombre des generations

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

//Définition de la structures de paramètres de fonction thread_execute
typedef struct {
int nb_th;
int num_th;
char *m;
char *bis;
int nb_l;
int nb_c;
} Tdonnees;

// focntion de mise à jour de la matrice 
void updateV1(char *m, char *bis, int l, int c,const int num_th,const int nb_th){
    for(int i = num_th*l/nb_th; i<(num_th+1)*l/nb_th ; i++){
        for(int j = 0; j<c ; j++){
            *(bis + i * c +j) = update_cell(m,i,j,l,c);
        }
    }
    for(int i = num_th*l/nb_th; i<(num_th+1)*l/nb_th ; i++){
        for(int j = 0; j<c ; j++){
		*(m + i * c +j) = *(bis + i * c +j);
        }
    }
}

int cpt_threads;
// la fonction de mise à jour passée a chaque thread
void * thread_execute(void *param){
    Tdonnees * donnee = (Tdonnees *) param ;
    for (int i=0; i<nb_generations ; i++){
        updateV1( donnee->m,donnee->bis,donnee->nb_l,donnee->nb_c,donnee->num_th,donnee->nb_th);	
        pthread_mutex_lock(&mutex);
        cpt_threads++;
    if(cpt_threads < donnee->nb_th){
        pthread_cond_wait(&cv,&mutex);
    }else{
        pthread_cond_broadcast(&cv);
        cpt_threads = 0;
    }
        pthread_mutex_unlock(&mutex);
    }
    free(donnee);
    pthread_exit(NULL);
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
    printf("BIENVENU AU JEU DE LA VIE\n");
    float temps;
    clock_t t1, t2;
    char matrix[maxl][maxc] = {dead};
    char bis[maxl][maxc]= {dead};

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
    set_matrix(&matrix[0][0], maxl, maxc, &canon[0][0], 9, 36, 10, 10);
    int i=0;

    //Définition de nombre de threads
    int nb_threads=4;
    pthread_t tab [nb_threads];

    //initailiser le structure qui contient les parameteres de fonction thread_execute et créer les threads
    Tdonnees * mat;
	for(int j =0; j<nb_threads; j++){
        mat = malloc (sizeof(Tdonnees));
        mat->nb_l = maxl;
        mat->nb_c = maxc;
        mat->nb_th = nb_threads;
        mat->num_th = j ;
        mat->m = & matrix[0][0];
        mat->bis=& bis[0][0];
        t1 = clock();
        pthread_create(&tab[j], NULL , thread_execute, (void * ) mat);
	}
  	
  	for(int j =0 ; j<nb_threads ; j++ ) 
  	{	
        //joindre les threads
  		pthread_join (tab[j],NULL);
  	} 
  	
    //Libérer les resources mutex et cv
  	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cv);
	t2 = clock();
    //calclue de temps d'éxecution
	temps = (float)(t2-t1)/CLOCKS_PER_SEC;
    printf("temps = %f\n", temps);
    return 0;
}