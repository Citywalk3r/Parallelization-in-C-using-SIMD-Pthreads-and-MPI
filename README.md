# Parallelization-in-C-using-SIMD-Pthreads-and-MPI

Assignment to use SIMD, Pthreads and MPI in conjunction, to efficiently parallelize a C code that calculates max, min and avg from float data in memory. 

-Baseline is the code given in the assignment.

-Unrolled & Unrolled and Jammed use these techniques to improve slightly the sequencial code.

-SSE uses SIMD commands for parallelization, SSE scrambled uses a different order on the same commands to improve the pipeline and    SSE MLT is an attempt to change the memory layout for further improvement (did not achieve performance gains).

-Pthreads further improve the SSE scrambled code with the intoduction of multi-threading.

-MPI is an (unsuccessful) attempt to add another level of parallelization using multiple processes.
