seed=1
m=1000
n=1000
l=4000
print=0
nthr=2

echo "You chose to use seed=$seed, m=$m, n=$n, l=$l, print=$print, number of threads=$nthr"
rm -rf serial omp_par_a omp_par_b omp_par_c pthr_par_a pthr_par_b pthr_par_c
gcc -fopenmp -o serial serial.c
gcc -fopenmp -o omp_par_a omp_par_a.c
gcc -fopenmp -o omp_par_b omp_par_b.c
gcc -fopenmp -o omp_par_c omp_par_c.c
gcc -pthread -fopenmp -o pthr_par_a pthr_par_a.c
gcc -pthread -fopenmp -o pthr_par_b pthr_par_b.c
gcc -pthread -fopenmp -o pthr_par_c pthr_par_c.c

echo "#############################################################"
echo "Serial Code Run"
./serial "$seed" "$m" "$n" "$l" "$print"
echo "#############################################################"
echo "OpenMP Parallel Code Run (a)"
#./omp_par_a "$seed" "$m" "$n" "$l" "$print" "$nthr"
echo "#############################################################"
echo "OpenMP Parallel Code Run (b)"
#./omp_par_b "$seed" "$m" "$n" "$l" "$print" "$nthr"
echo "#############################################################"
echo "OpenMP Parallel Code Run (c)"
#./omp_par_c "$seed" "$m" "$n" "$l" "$print" "$nthr"
echo "#############################################################"
echo "Pthreads Parallel Code Run (a)"
./pthr_par_a "$seed" "$m" "$n" "$l" "$print" "$nthr"
echo "#############################################################"
echo "Pthreads Parallel Code Run (b)"
./pthr_par_b "$seed" "$m" "$n" "$l" "$print" "$nthr"
echo "#############################################################"
echo "Pthreads Parallel Code Run (c)"
./pthr_par_c "$seed" "$m" "$n" "$l" "$print" "$nthr"
