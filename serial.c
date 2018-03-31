#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

char *rand_string(char *str, size_t size);
int **hamm;

void main(int argc, char *argv[])
{
  double start_time, run_time;
  int i, j, k, m, n, l, seed, print, dist=0;
  seed = atoi(argv[1]);
  srand(seed);
  m = atoi(argv[2]);
  n = atoi(argv[3]);
  l = atoi(argv[4]);
  print = atoi(argv[5]);

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

  // Calculate the Hamming distance of the strings and fill the m*n matrix
 hamm=malloc(m*sizeof(int *));
 for(int g=0;g<m;g++){
   hamm[g]=malloc(n*sizeof(int));
 }
  start_time= omp_get_wtime();

  for(j=0;j<m;j++){
    for(k=0;k<n;k++){
      for(i=0;i<l;i++){
        dist+=(str_set_A[j][i] != str_set_B[k][i]);
      }
      hamm[j][k]=dist;
      dist=0;
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


/*** Function to produce random binary string ***/

char *rand_string(char *str, size_t size)
{
  const char charset[] = "01";

  for (int n = 0; n < size; n++){
    int key = rand() % 2;
    str[n] = charset[key];
  }
  str[size] = '\0';

  return str;
}
