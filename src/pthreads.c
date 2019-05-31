#include "pthreads.h"

void sense_reversal_barrier_init(int num_threads){
	int i;
	sense = true;
	count = num_threads;
	
	if (localsense_list == NULL){
		localsense_list = (localsense_t *)malloc(sizeof(localsense_t)*\
												(unsigned long)num_threads);
		assert(localsense_list != NULL);
	}
	
	for (i = 0; i < num_threads; i++){
		localsense_list[i].lsense = true;
	}
}

void sense_reversal_barrier(int tid, int num_threads){
	int threadno = tid;
	localsense_list[threadno].lsense = !localsense_list[threadno].lsense;

	if (__sync_fetch_and_sub (&count, 1) == 1) {
		count = num_threads;
		sense = localsense_list[threadno].lsense;
	}
	else {
		while(sense != localsense_list[threadno].lsense) __sync_synchronize();
	}
}

void sense_reversal_barrier_destroy(void){
	if (localsense_list != NULL)
		free(localsense_list);

	localsense_list = NULL;
}

void initializeThreadData(threadData_t *cur, int i, int threads){
	cur->threadID = i;
	cur->threadTOTAL = threads;
	cur->threadBARRIER = 0;
	cur->threadOP = BUSYWAIT;
	cur->omegaData.i_start = 0;
	cur->omegaData.N = 0;
	cur->omegaData.maxF = 0.0f;
	cur->omegaData.minF = FLT_MAX;
	cur->omegaData.avgF = 0.0f;
	cur->omegaData.LVec = NULL;
	cur->omegaData.RVec = NULL;
	cur->omegaData.mVec = NULL;
	cur->omegaData.nVec = NULL;
	cur->omegaData.CVec = NULL;
	cur->omegaData.FVec = NULL;
}

void setThreadArgs(threadData_t * threadData, int tid, unsigned int i_start,\
					unsigned int N, float *LVec, float *RVec, float *mVec,\
					float *nVec, float *CVec, float *FVec){
	threadData[tid].omegaData.i_start = i_start;
	threadData[tid].omegaData.N = N;
	threadData[tid].omegaData.maxF = 0.0f;
	threadData[tid].omegaData.minF = FLT_MAX;
	threadData[tid].omegaData.avgF = 0.0f;
	threadData[tid].omegaData.LVec = LVec;
	threadData[tid].omegaData.RVec = RVec;
	threadData[tid].omegaData.mVec = mVec;
	threadData[tid].omegaData.nVec = nVec;
	threadData[tid].omegaData.CVec = CVec;
	threadData[tid].omegaData.FVec = FVec;
}

void updateThreadArgs(threadData_t * threadData, unsigned int i_start,\
					unsigned int N, float *LVec, float *RVec, float *mVec,\
					float *nVec, float *CVec, float *FVec){
	int threadIndex = 0;
	for(threadIndex=0;threadIndex<threadData->threadTOTAL;threadIndex++)
		setThreadArgs(threadData, threadIndex, i_start, N, LVec, RVec, mVec,\
						nVec, CVec, FVec);
}

static inline void syncThreadsBARRIER(threadData_t *threadData){
	int threads = threadData[0].threadTOTAL;
	threadData[0].threadOP = BUSYWAIT;
	sense_reversal_barrier(0, threads);
}

static inline void computeOmega(threadData_t *threadData){
	
	int threadID = threadData[0].threadID;
	int totalThreads = threadData[0].threadTOTAL;
	
	/*
	 * Calculating the part of the loop that each thread processes.
	 * The formula calculates the i and N even for shifted blocks (like [20:60])
	 * this functionality was needed for MPI, otherwise the calculations were 
	 * much simpler.
	 * 
	 */
	unsigned int i = (threadID *\
				(threadData[0].omegaData.N - threadData[0].omegaData.i_start)\
				/ totalThreads) + threadData[0].omegaData.i_start;
	unsigned int N = ((threadID+1) *\
				(threadData[0].omegaData.N - threadData[0].omegaData.i_start)\
				/ totalThreads) + threadData[0].omegaData.i_start;
	
// 	printf("In thread [%d] i is %d, where i_start is %d\n", threadID, i,\
// 			threadData[0].omegaData.i_start);
// 	printf("In thread [%d] N is %d, where omega N is %d\n", threadID, N,\
// 			threadData[0].omegaData.N);
				
	float *LVec = threadData[0].omegaData.LVec;
	float *RVec = threadData[0].omegaData.RVec;
	float *mVec = threadData[0].omegaData.mVec;
	float *nVec = threadData[0].omegaData.nVec;
	float *CVec = threadData[0].omegaData.CVec;
	float *FVec = threadData[0].omegaData.FVec;
	float avgF = 0.0f;
	float maxF = 0.0f;
	float minF = FLT_MAX;

	__m128 maxF_mm = _mm_set1_ps(0.0f);
	__m128 avgF_mm = _mm_set1_ps(0.0f);
	__m128 minF_mm = _mm_set1_ps(FLT_MAX);
	
	for(i = i; i <= N-4; i+=4){
		__m128 one_vec = _mm_set1_ps(1.0f);
		__m128 two_vec = _mm_set1_ps(2.0f);
		__m128 centi_vec = _mm_set1_ps(0.01f);
		__m128 LVec_mm, RVec_mm, mVec_mm, nVec_mm, CVec_mm, FVec_mm;

		LVec_mm = _mm_load_ps(&LVec[i]);
		RVec_mm = _mm_load_ps(&RVec[i]);
		mVec_mm = _mm_load_ps(&mVec[i]);
		nVec_mm = _mm_load_ps(&nVec[i]);
		CVec_mm = _mm_load_ps(&CVec[i]);
		
		__m128 num_0 = _mm_add_ps(LVec_mm, RVec_mm);
		__m128 sub1 = _mm_sub_ps(mVec_mm, one_vec);
		__m128 sub0 = _mm_sub_ps(CVec_mm, LVec_mm); 
		__m128 sub2 = _mm_sub_ps(nVec_mm, one_vec);
		__m128 div1 = _mm_div_ps(sub1, two_vec);
		__m128 den_0 = _mm_sub_ps(sub0, RVec_mm);
		__m128 div2 = _mm_div_ps(sub2, two_vec);
		__m128 num_1 = _mm_mul_ps(mVec_mm, div1);
		__m128 num_2 = _mm_mul_ps(nVec_mm, div2);
		__m128 den_1 = _mm_mul_ps(mVec_mm, nVec_mm);
		__m128 add0 = _mm_add_ps(num_1, num_2);
		__m128 den = _mm_div_ps(den_0, den_1);
		__m128 num = _mm_div_ps(num_0, add0);
		__m128 add3 = _mm_add_ps(den, centi_vec);
		FVec_mm = _mm_div_ps(num, add3);

		maxF_mm = _mm_max_ps(maxF_mm, FVec_mm);
		minF_mm = _mm_min_ps(minF_mm, FVec_mm);
		avgF_mm = _mm_add_ps(avgF_mm, FVec_mm);
	}
	maxF = total_max(maxF_mm);
	minF = total_min(minF_mm);
	avgF = total_avg(avgF_mm);

	for(i = N-(N%4); i < N; i++){
		float num_0 = LVec[i] + RVec[i];
		float num_1 = mVec[i] * (mVec[i] - 1.0f) / 2.0f;
		float num_2 = nVec[i] * (nVec[i] - 1.0f) / 2.0f;
		float num = num_0 / (num_1 + num_2);
		float den_0 = CVec[i] - LVec[i] - RVec[i];
		float den_1 = mVec[i] * nVec[i];
		float den = den_0 / den_1;
		FVec[i] = num / (den + 0.01f);
		maxF = FVec[i] > maxF ? FVec[i] : maxF;
		minF = FVec[i] < minF ? FVec[i] : minF;
		avgF += FVec[i];
	}

	threadData[0].omegaData.maxF = maxF;
	threadData[0].omegaData.minF = minF;
	threadData[0].omegaData.avgF = avgF;
}

static inline void execFuncMaster(threadData_t *threadData, int op){
	if(op == OMEGA)
		computeOmega(&threadData[0]);
}

static inline void setThreadOP(threadData_t *threadData, int op){
	int i, threads = threadData[0].threadTOTAL;

	for(i = 0; i < threads; i++)
		threadData[i].threadOP = op;
}

void startThreadOPS(threadData_t *threadData, int op){
	setThreadOP(threadData, op);
	execFuncMaster(threadData, op);
	threadData[0].threadBARRIER=1;
	syncThreadsBARRIER(threadData);
}

static void pinToCore(int tid){
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);    
	CPU_SET(tid, &cpuset);
	
	if(pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0){
		fprintf(stdout, "\n ERROR: Please specify a number of threads that is \
smaller or equal\n        to the number of available physical cores (%d).\n\n",\
				tid);
		exit(0);
	}
}

void *thread(void *x){
	
	threadData_t *currentThread = (threadData_t *) x;

	int tid = currentThread->threadID;
 	pinToCore(tid);
	
	int threads = currentThread->threadTOTAL;

	while (1){
		__sync_synchronize();

		if(currentThread->threadOP == EXIT)
			return NULL;

		if(currentThread->threadOP == OMEGA){
			computeOmega(currentThread);
			currentThread->threadOP=BUSYWAIT;
			sense_reversal_barrier(tid, threads);
		}
	}
	return NULL;
}

void terminateWorkerThreads(pthread_t *workerThreadL, threadData_t *threadData){
	int i, threads=threadData[0].threadTOTAL;

	for(i = 0; i < threads; i++)
		threadData[i].threadOP = EXIT;
	for(i = 1; i < threads; i++)
		pthread_join(workerThreadL[i-1], NULL);
}
