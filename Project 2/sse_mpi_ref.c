#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include <assert.h>
#include <emmintrin.h>
#include <mpi.h>

double gettime(void) {
  struct timeval ttime;
  gettimeofday(&ttime , NULL);
  return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

float randpval () {
  int vr = rand();
  int vm = rand()%vr;
  float r = ((float)vm)/(float)vr;
  assert(r>=0.0 && r<=1.00001);
  return r;
}

int main(int argc, char ** argv) {
  int N = atoi(argv[1]);
  int iters = 1000;
  srand(1);

  //Start the MPI
  MPI_Init(NULL, NULL);

  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

  //Distribute load to the processes
  int proc_load = N/world_size;
  int proc_start = world_rank * proc_load;

  float * mVec = (float*)malloc(sizeof(float)*N);
  assert(mVec!=NULL);
  float * nVec = (float*)malloc(sizeof(float)*N);
  assert(nVec!=NULL);
  float * LVec = (float*)malloc(sizeof(float)*N);
  assert(LVec!=NULL);
  float * RVec = (float*)malloc(sizeof(float)*N);
  assert(RVec!=NULL);
  float * CVec = (float*)malloc(sizeof(float)*N);
  assert(CVec!=NULL);
  float * FVec = (float*)malloc(sizeof(float)*N);
  assert(FVec!=NULL);

  for(int i=0;i<N;i++) {
    mVec[i] = (float)(2+rand()%10);
    nVec[i] = (float)(2+rand()%10);
    LVec[i] = 0.0;
    for(int j=0;j<mVec[i];j++) {
      LVec[i] += randpval();
    }
    RVec[i] = 0.0;
    for(int j=0;j<nVec[i];j++) {
      RVec[i] += randpval();
    }
    CVec[i] = 0.0;
    for(int j=0;j<mVec[i]*nVec[i];j++) {
      CVec[i] += randpval();
    }
    FVec[i] = 0.0;

    assert(mVec[i]>=2.0 && mVec[i]<=12.0);
    assert(nVec[i]>=2.0 && nVec[i]<=12.0);
    assert(LVec[i]>0.0 && LVec[i]<=1.0*mVec[i]);
    assert(RVec[i]>0.0 && RVec[i]<=1.0*nVec[i]);
    assert(CVec[i]>0.0 && CVec[i]<=1.0*mVec[i]*nVec[i]);
  }

  double timeTotal = 0.0f;
  float maxF = 0.0f;
  float max[4];
  __m128 n0,n1,n2,n,d0,d1,d;
  __m128 maxFV = _mm_setzero_ps();
  __m128 z001 = _mm_set1_ps(0.01f);
  __m128 *LV = (__m128 *) LVec;
  __m128 *RV = (__m128 *) RVec;
  __m128 *CV = (__m128 *) CVec;
  __m128 *FV = (__m128 *) FVec;
  __m128 *mV = (__m128 *) mVec;
  __m128 *nV = (__m128 *) nVec;

  for(int j=0;j<iters;j++) {
    double time0 = gettime();
    for(int i = proc_start/4; i < proc_start/4 + proc_load/4; i++) {
      n0 = _mm_add_ps(LV[i],RV[i]);
      n1 = _mm_mul_ps(mV[i],(mV[i]-1.0)/2.0);
      n2 = _mm_mul_ps(nV[i],(nV[i]-1.0)/2.0);
      n = _mm_div_ps(n0,_mm_add_ps(n1,n2));
      d0 = _mm_sub_ps(CV[i],_mm_add_ps(LV[i],RV[i]));
      d1 = _mm_mul_ps(mV[i],nV[i]);
      d = _mm_div_ps(d0,d1);

      FV[i] = _mm_div_ps(n,_mm_add_ps(d,z001));
      maxFV = _mm_max_ps(FV[i], maxFV);
    }
    double time1=gettime();
    timeTotal += time1-time0;
  }

  _mm_store_ps(max, maxFV);
  maxF = MAX(max[0], MAX(max[1], MAX(max[2], max[3])));

  float *max_coll = NULL;
  double *time_coll = NULL;
  if(world_rank == 0) {
      max_coll = (float *)malloc(world_size * sizeof(float));
      assert(max_coll != NULL);
      time_coll = (double *)malloc(world_size * sizeof(double));
      assert(time_coll != NULL);
  }

  MPI_Gather(&timeTotal, 1, MPI_DOUBLE, time_coll, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Gather(&maxF, 1, MPI_FLOAT, max_coll, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

  if(world_rank == 0) {
      float final_maxF = -1.0f;
      float final_time = 0.0f;
      for (int i = 0; i < world_size; i++) {
        final_time = time_coll[i];
        final_maxF = MAX(max_coll[i], final_maxF);
      }
      printf("Time %f Max %f\n", final_time/(iters), final_maxF);
  }

  free(max_coll);
  free(time_coll);
  free(mVec);
  free(nVec);
  free(LVec);
  free(RVec);
  free(CVec);
  free(FVec);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
