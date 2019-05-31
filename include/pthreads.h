/*
 * Functions used by pthreads
 * Special thanks to our instructor, N. Alachiotis @https://github.com/alachins/
 * 	for providing an awesome basis for the threads and barriers 
 */

#ifndef GLOBAL_H
#include "generic.h"
#endif

#ifndef SSE_H
#include "sse.h"
#endif

#ifndef PTHREADS_H

#define PTHREADS_H
#include <pthread.h>
#include <unistd.h>
#define EXIT 127
#define BUSYWAIT 0
#define OMEGA 1

unsigned int THREADS;

/* Helper global vars for the sense reversal barrier */
static pthread_t *workerThread; 
static int count;
static bool sense;

/* Sense of each thread */
typedef struct localsense_t {
	bool lsense;
}localsense_t;

/* Sense reversal barrier functions */
static localsense_t *localsense_list = NULL;
void sense_reversal_barrier_init(int num_threads);
void sense_reversal_barrier(int tid, int num_threads);
void sense_reversal_barrier_destroy(void);

/* 
 * Calculation data for each thread. They mostly use the [0] thread's values
 * 	as only max, min and avg are actually unique for each thread
 * Everything else is just pointers to-be-used in the threads
 * 
 */
typedef struct{
	unsigned int i_start;
	unsigned int N;
	float *LVec;
	float *RVec;
	float *mVec;
	float *nVec;
	float *CVec;
	float *FVec;
	float maxF;
	float minF;
	float avgF;
}omegaData_t;

/*
 * Thread data. These data are unique to each thread, and include variables for:
 * 		the id,
 *		the total number of threads (this doesn't need to be unique),
 * 		if they are on a barrier,
 * 		the current operation of the thread
 * 		and the above data (omegaData). 
 * 
 */
typedef struct{
	int threadID;
	int threadTOTAL;
	int threadBARRIER;
	int threadOP;
	omegaData_t omegaData;
}threadData_t;

/*
 * Thread functions. These include:
 *		the initialization,
 *		the barrier syncing,
 * 		the operation function that each thread runs for the omega calculation,
 * 		the master function that assigns operations in each thread (only one
 * 			operation here, could be removed, stayed during developement in
 *			case it is needed, not removed for future scalability)
 * 		the actual master function, which starts the threads
 * 		the thread function, which includes barriers (busy-wait mode)
 * 		the termination of the workerThreads (threads != 0)
 * 
 */
void initializeThreadData(threadData_t *cur, int i, int threads);
void setThreadArgs(threadData_t * threadData, int tid, unsigned int i_start,\
					unsigned int N, float *LVec, float *RVec, float *mVec,\
					float *nVec, float *CVec, float *FVec);
void updateThreadArgs(threadData_t * threadData, unsigned int i_start,\
					unsigned int N, float *LVec, float *RVec, float *mVec,\
					float *nVec, float *CVec, float *FVec);
static inline void syncThreadsBARRIER(threadData_t *threadData);
static inline void computeOmega(threadData_t *threadData);
static inline void execFuncMaster(threadData_t *threadData, int op);
void startThreadOPS(threadData_t *threadData, int op);
void *thread(void *x);
void terminateWorkerThreads(pthread_t *workerThreadL, threadData_t *threadData);

#endif
