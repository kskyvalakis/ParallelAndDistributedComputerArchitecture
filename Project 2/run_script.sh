P=2
N=100

rm -rf ref sse_ref sse_mpi_ref
gcc ref.c -o ref
gcc -msse4.2 sse_ref.c -o sse_ref
mpicc -msse4.2 sse_mpi_ref.c -o sse_mpi_ref

echo "#############################################################"
echo "Serial Reference Code Run"
./ref "$N"
echo "#############################################################"
echo "SSE Reference Code Run"
./sse_ref "$N"
echo "#############################################################"
echo "SSE+MPI Reference Code Run"
mpiexec -n "$P" ./sse_mpi_ref "$N"
