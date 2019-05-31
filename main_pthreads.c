#include "pthreads.h"

int main(int argc, char ** argv){
	assert(argc == 3);
	
	double timeTotalMainStart = gettime();

	float avgF = 0.0f;
	float maxF = 0.0f;
	float minF = FLT_MAX;

	unsigned int N = (unsigned int)atoi(argv[1]);
	THREADS = (unsigned int)atoi(argv[2]);
	unsigned int iters = 10;

	srand(1);

	float* mVec = (float*)malloc(sizeof(float) * N);
	assert(mVec != NULL);

	float* nVec = (float*)malloc(sizeof(float) * N);
	assert(nVec != NULL);

	float* LVec = (float*)malloc(sizeof(float) * N);
	assert(LVec != NULL);

	float* RVec = (float*)malloc(sizeof(float) * N);
	assert(RVec != NULL);

	float* CVec = (float*)malloc(sizeof(float) * N);
	assert(CVec != NULL);

	float* FVec = (float*)malloc(sizeof(float) * N);
	assert(FVec != NULL);

	for(unsigned int i = 0; i < N; i++){
		mVec[i] = (float)(MINSNPS_B + rand() % MAXSNPS_E);
		nVec[i] = (float)(MINSNPS_B + rand() % MAXSNPS_E);
		LVec[i] = randpval() * mVec[i];
		RVec[i] = randpval() * nVec[i];
		CVec[i] = randpval() * mVec[i]*nVec[i];
		FVec[i] = 0.0;
		assert(mVec[i] >= MINSNPS_B && mVec[i] <= (MINSNPS_B + MAXSNPS_E));
		assert(nVec[i] >= MINSNPS_B && nVec[i] <= (MINSNPS_B + MAXSNPS_E));
		assert(LVec[i] >= 0.0f && LVec[i] <= 1.0f * mVec[i]);
		assert(RVec[i] >= 0.0f && RVec[i] <= 1.0f * nVec[i]);
		assert(CVec[i] >= 0.0f && CVec[i] <= 1.0f * mVec[i] * nVec[i]);
	}
	
	int threads = THREADS;
	sense_reversal_barrier_init(threads);
	assert(N>threads);
	workerThread = NULL;
	workerThread = (pthread_t *) malloc(sizeof(pthread_t)*\
										((unsigned long)(threads-1)));
	
	threadData_t *threadData = (threadData_t *) malloc(sizeof(threadData_t)*\
													((unsigned long)threads));
	assert(threadData!=NULL);
	
	for(int i = 0; i < threads; i++)
		initializeThreadData(&threadData[i], i, threads);

	for(int i = 1; i < threads; i++)
		pthread_create(&workerThread[i-1], NULL, thread,\
						(void *)(&threadData[i]));
	
	double timeOmegaTotalStart = gettime();

	for(unsigned int j = 0; j < iters; j++){
		avgF = 0.0f;
		maxF = 0.0f;
		minF = FLT_MAX;
		
		/* Executing the threads */
		updateThreadArgs(&threadData[0], 0, N, LVec, RVec,\
								mVec, nVec, CVec, FVec);
		startThreadOPS(threadData, OMEGA);
		
		/* Gathering the results */
		for(int i = 0; i < threads; i++){
			maxF = threadData[i].omegaData.maxF > maxF ?\
					threadData[i].omegaData.maxF : maxF;
			minF = threadData[i].omegaData.minF < minF ?\
					threadData[i].omegaData.minF : minF;
			avgF += threadData[i].omegaData.avgF;
		}
		
		/* serial execution of residual elements (when N % threads != 0) */
		for(int i = N-(N%threads); i < N; i++){
			
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
	}
	double timeOmegaTotal = gettime() - timeOmegaTotalStart;
	double timeTotalMainStop = gettime();

	printf("Omega time %fs - Total time %fs - Min %e - Max %e - Avg %e\n",\
			timeOmegaTotal/iters, timeTotalMainStop-timeTotalMainStart,\
			(double)minF, (double)maxF, (double)avgF/N);

	free(mVec);
	free(nVec);
	free(LVec);
	free(RVec);
	free(CVec);
	free(FVec);
	terminateWorkerThreads(workerThread,threadData);
	sense_reversal_barrier_destroy();
	if(threadData!=NULL)
		free(threadData);
	threadData = NULL;
}
