#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>
#include <assert.h>
#include <emmintrin.h>

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
  int iters = 1000,k;
  srand(1);

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
  double maxF = 0.0f;
  float max[4];
  __m128 n0,n1,n2,n,d0,d1,d,LV,RV,CV,nV,mV;
  __m128 FV = _mm_setzero_ps();
  __m128 maxFV = _mm_setzero_ps();
  __m128 z001 = _mm_set1_ps(0.01f);

  for(int j=0;j<iters;j++) {
    double time0=gettime();
    for(int i=0;i<N;i+=4) {
      LV = _mm_load_ps(&LVec[i]);
      RV = _mm_load_ps(&RVec[i]);
      n0 = _mm_add_ps(LV,RV);
      mV = _mm_load_ps(&mVec[i]);
      n1 = _mm_mul_ps(mV,(mV-1.0)/2.0);
      nV = _mm_load_ps(&nVec[i]);
      n2 = _mm_mul_ps(nV,(nV-1.0)/2.0);
      n = _mm_div_ps(n0,_mm_add_ps(n1,n2));

      CV = _mm_load_ps(&CVec[i]);
      d0 = _mm_sub_ps(CV,_mm_add_ps(LV,RV));
      d1 = _mm_mul_ps(mV,nV);
      d = _mm_div_ps(d0,d1);

      FV = _mm_div_ps(n,_mm_add_ps(d,z001));
      maxFV = _mm_max_ps(FV, maxFV);
    }
    double time1=gettime();
    timeTotal += time1-time0;
  }

  _mm_store_ps(max, maxFV);
  maxF = MAX(max[0], MAX(max[1], MAX(max[2], max[3])));

  printf("Time: %f Max: %lf\n", timeTotal/iters,maxF);

  free(mVec);
  free(nVec);
  free(LVec);
  free(RVec);
  free(CVec);
  free(FVec);
}
