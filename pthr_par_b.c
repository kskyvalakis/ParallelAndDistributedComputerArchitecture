#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>

struct ThreadData {
  int id;
  char* A;
  char** B;
  int start, stop;
  int length_B,length_char;
  int outer_var;
};

int** hamm;
char* rand_string(char *str, size_t size);
void* hamming(void * td);

void main(int argc, char *argv[])
{
  double start_time, run_time;
  int i, j, k, m, n, l, seed, print, dist=0, nthr;
  seed = atoi(argv[1]);
  srand(seed);
  m = atoi(argv[2]);
  n = atoi(argv[3]);
  l = atoi(argv[4]);
  print = atoi(argv[5]);
  nthr = atoi(argv[6]);

  // Generate m strings for set A
  char *str_set_A[m];
  for(i=0;i<m;i++){
    str_set_A[i]=malloc(l*sizeof(char *));
    strcpy(str_set_A[i],rand_string(str_set_A[i],l));
  }

  // Generate n strings for set B
  char *str_set_B[n];
  for(i=0;i<n;i++){
    str_set_B[i]=malloc(l*sizeof(char *));
    strcpy(str_set_B[i],rand_string(str_set_B[i],l));
  }

  // Threads initializations
  pthread_t thread[nthr];
  struct ThreadData data[nthr];
  int tasksPerString=(n+nthr-1)/nthr;

  // Calculate the Hamming distance of the strings and fill the m*n matrix
  int ret;
  hamm=malloc(m*sizeof(int *));
  for(int g=0;g<m;g++){
    hamm[g]=malloc(n*sizeof(int));
  }
  start_time=omp_get_wtime();

  for(j=0;j<m;j++){
    /* Divide work for threads, prepare parameters */
    for (i=0; i<nthr; i++){
      data[i].id=i;
      data[i].start=i*tasksPerString;
      data[i].stop=(i+1)*tasksPerString;
      data[i].A=str_set_A[j];
      data[i].length_char=l;
      data[i].outer_var=j;
      data[i].B=str_set_B;
      data[i].length_B=n;
    }
    data[nthr-1].stop=n;

    for(int id=0;id<nthr;id++){
      pthread_create(&thread[id],NULL,hamming,(void* ) &data[id]);
    }

    for(int id=0;id<nthr;id++){
      pthread_join(thread[id],NULL);
    }
  }

  run_time = omp_get_wtime() - start_time;
  printf("Time: %f seconds \n",run_time);

  if(print){
    for(i=0;i<m;i++){
      for(j=0;j<n;j++){
        printf("%d ",hamm[i][j]);
        if(j==n-1) printf("\n");
      }
    }
  }

}

/*** Function to execute thread computations ***/

void* hamming(void* td){
  struct ThreadData* data;
  data=(struct ThreadData*) td;
  int id=data->id;
  char* Af=data->A;
  int start=data->start;
  int stop=data->stop;
  int dist=0, j,i;
  int length_B=data->length_B;
  int outer=data->outer_var;
  int l=data->length_char;
  char** Bf=data->B;

  for(j=start;j<stop;j++){
    for (i=0; i<l; i++) {
      dist+=(Af[i] != Bf[j][i]);
    }
    hamm[outer][j]  =dist;
    dist=0;
  }

  return NULL;
}

/*** Function to produce random binary string ***/

char *rand_string(char *str, size_t size){
  const char charset[] = "01";

  for (int n = 0; n < size; n++){
    int key = rand() % 2;
    str[n] = charset[key];
  }
  str[size] = '\0';

  return str;
}
