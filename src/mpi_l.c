#include "mpi_l.h"

float * MPI_Func(unsigned int i_s, unsigned int N, int THREADS, float *maxF,\
				float *minF, float *avgF, double *timeOmegaTotalStart){

	srand(1);
// 	for(int i = i_s; i < N; i++){
// 		*maxF = (float)i > *maxF ? (float)i : *maxF;
// 		*minF = (float)i < *minF ? (float)i : *minF;
// 		*avgF += 1.0f;
// 	}
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

	for(unsigned int i = i_s; i < N; i++){
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
	
	*timeOmegaTotalStart = gettime();
	
// 	int threads = THREADS;
// 	sense_reversal_barrier_init(threads);
// 	assert(N>threads);
// 	workerThread = NULL;
// 	workerThread = (pthread_t *) malloc(sizeof(pthread_t)*\
// 										((unsigned long)(threads-1)));
// 	
// 	threadData_t *threadData = (threadData_t *) malloc(sizeof(threadData_t)*\
// 													((unsigned long)threads));
// 	assert(threadData!=NULL);
// 
// 	for(int i = 0; i < threads; i++)
// 		initializeThreadData(&threadData[i], i, threads);
// 
// 	for(int i = 1; i < threads; i++)
// 		pthread_create(&workerThread[i-1], NULL, thread,\
// 						(void *)(&threadData[i]));
// 	
// 	/* Executing the threads */
// 	updateThreadArgs(threadData, i_s, N, LVec, RVec, mVec, nVec, CVec, FVec);
// 	startThreadOPS(threadData, OMEGA);
// 	for(int i = 0; i < threads; i++){
// 		*maxF = threadData[i].omegaData.maxF > *maxF ?\
// 				threadData[i].omegaData.maxF : *maxF;
// 		*minF = threadData[i].omegaData.minF < *minF ?\
// 				threadData[i].omegaData.minF : *minF;
// 		*avgF += threadData[i].omegaData.avgF;
// 	}
// 	
// 	/* serial execution of residual elements (when N % threads != 0) */
// 	for(int i = N-(N%threads); i < N; i++){
// 		float num_0 = LVec[i] + RVec[i];
// 		float num_1 = mVec[i] * (mVec[i] - 1.0f) / 2.0f;
// 		float num_2 = nVec[i] * (nVec[i] - 1.0f) / 2.0f;
// 		float num = num_0 / (num_1 + num_2);
// 		float den_0 = CVec[i] - LVec[i] - RVec[i];
// 		float den_1 = mVec[i] * nVec[i];
// 		float den = den_0 / den_1;
// 		FVec[i] = num / (den + 0.01f);
// 		*maxF = FVec[i] > *maxF ? FVec[i] : *maxF;
// 		*minF = FVec[i] < *minF ? FVec[i] : *minF;
// 		*avgF += FVec[i];
// 	}
	
	/* 
		* Editing this part of code 
		*
		* __m128: vector of 4 single precision floating point (32bit) elements 
		* _mm_set1_ps -> set all elements to value (__m128 vectors specific)
		* _mm_load_ps -> fill a __m128 vector with values from an array
		* _mm_add_ps -> add two __m128 vectors (sub,mul,div are the same)
		* _mm_max_ps -> find max in pairs of elements of two __m128 vectors
		* 				 (same for min)
		* 
		* If a command writes to a buffer that is not read in the next command,
		* they are both executed simultaneously (used whenever possible)
		*/

	/* vectorize all static numbers to use in operations */
	__m128 one_vec = _mm_set1_ps(1.0f);
	__m128 two_vec = _mm_set1_ps(2.0f);
	__m128 centi_vec = _mm_set1_ps(0.01f);
	/* creating vectors for all the variables */
	__m128  LVec_mm, RVec_mm, mVec_mm, nVec_mm, CVec_mm, FVec_mm;
	
	/* zero-initialization of maxF and avgF */
	__m128 maxF_mm = _mm_set1_ps(0.0f);
	__m128 avgF_mm = _mm_set1_ps(0.0f);
	/* max-initialization of minF */
	__m128 minF_mm = _mm_set1_ps(FLT_MAX);

	for(unsigned int i = i_s; i <= N-4; i+=4){
		/* filling the vectors with the appropriate variable elements */
		LVec_mm = _mm_load_ps(&LVec[i]);
		RVec_mm = _mm_load_ps(&RVec[i]);
		mVec_mm = _mm_load_ps(&mVec[i]);
		nVec_mm = _mm_load_ps(&nVec[i]);
		CVec_mm = _mm_load_ps(&CVec[i]);
		/* 
			* expressing operations with vector arithmetics, one instruction 
			* at a time, where the operation num=a*(b-1)/2 becomes:
			* sub = b-1
			* div = sub/2
			* num = a*div 
			*/
		
		/* Code before simultaneous executions */
		
		/* num_0 = LVec[i] + RVec[i] */
		__m128 num_0 = _mm_add_ps(LVec_mm, RVec_mm);
		/* num_1 = mVec[i] * (mVec[i] - 1.0f) / 2.0f */
		__m128 sub1 = _mm_sub_ps(mVec_mm, one_vec);
		__m128 div1 = _mm_div_ps(sub1, two_vec);
		__m128 num_1 = _mm_mul_ps(mVec_mm, div1);
		/* num_2 = nVec[i] * (nVec[i] - 1.0f) / 2.0f */
		__m128 sub2 = _mm_sub_ps(nVec_mm, one_vec);
		__m128 div2 = _mm_div_ps(sub2, two_vec);
		__m128 num_2 = _mm_mul_ps(nVec_mm, div2);
		/* num = num_0 / (num_1 + num_2) */
		__m128 add0 = _mm_add_ps(num_1, num_2);
		__m128 num = _mm_div_ps(num_0, add0);
		/* den_0 = CVec[i] - LVec[i] - RVec[i] */
		__m128 sub0 = _mm_sub_ps(CVec_mm, LVec_mm);
		__m128 den_0 = _mm_sub_ps(sub0, RVec_mm);
		/* den_1 = mVec[i] * nVec[i] */
		__m128 den_1 = _mm_mul_ps(mVec_mm, nVec_mm);
		/* den = den_0 / den_1 */
		__m128 den = _mm_div_ps(den_0, den_1);
		/* FVec[i] = num / (den + 0.01f) */
		__m128 add3 = _mm_add_ps(den, centi_vec);
		FVec_mm = _mm_div_ps(num, add3);
	
		/* maxF = FVec[i] > maxF ? FVec[i] : maxF */
		maxF_mm = _mm_max_ps(maxF_mm,FVec_mm);
		/* minF = FVec[i] < minF ? FVec[i] : minF */
		minF_mm = _mm_min_ps(minF_mm,FVec_mm);
		/* avgF += FVec[i] */
		avgF_mm = _mm_add_ps(avgF_mm, FVec_mm);
		
		*maxF = total_max(maxF_mm);
		*minF = total_min(minF_mm);
		*avgF = total_avg(avgF_mm);
		
		/* serial execution of residual elements (when N % 4 != 0) */
		for(int i = N-(N%4); i < N; i++){
			float num_0 = LVec[i] + RVec[i];
			float num_1 = mVec[i] * (mVec[i] - 1.0f) / 2.0f;
			float num_2 = nVec[i] * (nVec[i] - 1.0f) / 2.0f;
			float num = num_0 / (num_1 + num_2);
			float den_0 = CVec[i] - LVec[i] - RVec[i];
			float den_1 = mVec[i] * nVec[i];
			float den = den_0 / den_1;
			FVec[i] = num / (den + 0.01f);
			*maxF = FVec[i] > *maxF ? FVec[i] : *maxF;
			*minF = FVec[i] < *minF ? FVec[i] : *minF;
			*avgF += FVec[i];
		}
	}
	
	
	
	free(mVec);
	free(nVec);
	free(LVec);
	free(RVec);
	free(CVec);
	free(FVec);
// 	terminateWorkerThreads(workerThread,threadData);
// 	sense_reversal_barrier_destroy();
// 	if(threadData!=NULL)
// 		free(threadData);
// 	threadData = NULL;
}
