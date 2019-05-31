/*
 * Functions used by MPI
 */

#ifndef GLOBAL_H
#include "generic.h"
#endif

#ifndef SSE_H
#include "sse.h"
#endif

#ifndef PTHREADS_H
#include "pthreads.h"
#endif

#ifndef MPI_H

#define MPI_H
#include <mpi.h>

/* 
 * MPI Function that uses part of the previous main function to 
 * 	make processes. This includes pthreads and the scrambled version of SSE.
 * 
 */
float * MPI_Func(unsigned int i_s, unsigned int N, int THREADS, float *maxF,\
				float *minF, float *avgF, double *timeOmegaTotalStart);

#endif
