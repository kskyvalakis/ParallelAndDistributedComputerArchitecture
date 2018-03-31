# Parallel-and-Distributed-Computer-Architecture

Parameter Information & Set-upnum

seed : seed to be used for the pseudorandom function
m : number of strings in set A
n : number of strings in set B
l : length of each string
print : print flag for outputting the m x n Hamming distance matrix or not
nthr : number of threads to use

Open run_script.sh and alter the values of these parameters before execution to observe changes in performance.

Occasionally the following command may be needed in order to avoid stack memory overflows : ulimit -s unlimited.
The default stack memory size is 8192 kB and sometimes that is not enough to store all of our strings so by setting
it to unlimited we can fully exploit our memory.

Execution : ./run_script.sh
