#include "sse.h"

int main(int argc, char ** argv){
	assert(argc == 2);

	double timeTotalMainStart = gettime();

	float avgF = 0.0f;
	float maxF = 0.0f;
	float minF = FLT_MAX;

	unsigned int N = (unsigned int)atoi(argv[1]);

	unsigned int iters = 10;

	srand(1);
	
	/* (N/4)+1 to account for the N%4 */
	float* memblock = (float*)malloc(sizeof(float)*6*N);
	float** mVec = (float**)malloc(sizeof(float*)*N);
	float** FVec = (float**)malloc(sizeof(float*)*N);
	float** nVec = (float**)malloc(sizeof(float*)*N);
	float** LVec = (float**)malloc(sizeof(float*)*N);
	float** RVec = (float**)malloc(sizeof(float*)*N);
	float** CVec = (float**)malloc(sizeof(float*)*N);

	for(int i = 0; i <= 6*(N-4); i+=6*4){
		for(int j = 0; j < 4; j++){
			mVec[i/6+j] = &memblock[i+j];
			nVec[i/6+j] = &memblock[i+j+4];
			LVec[i/6+j] = &memblock[i+j+8];
			RVec[i/6+j] = &memblock[i+j+12];
			CVec[i/6+j] = &memblock[i+j+16];
			FVec[i/6+j] = &memblock[i+j+20];
		}
	}
	for(int i = 6*(N-N%4); i < 6*N; i+=6){
		mVec[i/6] = &memblock[i];
		nVec[i/6] = &memblock[i+4];
		LVec[i/6] = &memblock[i+8];
		RVec[i/6] = &memblock[i+12];
		CVec[i/6] = &memblock[i+16];
		FVec[i/6] = &memblock[i+20];
	}
	assert(mVec!=NULL);
	assert(nVec!=NULL);
	assert(LVec!=NULL);
	assert(RVec!=NULL);
	assert(CVec!=NULL);
	assert(FVec!=NULL);
	
	for(unsigned int i = 0; i < N; i++){
		mVec[i][0] = (float)(MINSNPS_B + rand() % MAXSNPS_E);
		nVec[i][0] = (float)(MINSNPS_B + rand() % MAXSNPS_E);
		LVec[i][0] = randpval() * mVec[i][0];
		RVec[i][0] = randpval() * nVec[i][0];
		CVec[i][0] = randpval() * mVec[i][0]*nVec[i][0];
		FVec[i][0] = 0.0;
		assert(mVec[i][0] >= MINSNPS_B && mVec[i][0] <= (MINSNPS_B + MAXSNPS_E));
		assert(nVec[i][0] >= MINSNPS_B && nVec[i][0] <= (MINSNPS_B + MAXSNPS_E));
		assert(LVec[i][0] >= 0.0f && LVec[i][0] <= 1.0f * mVec[i][0]);
		assert(RVec[i][0] >= 0.0f && RVec[i][0] <= 1.0f * nVec[i][0]);
		assert(CVec[i][0] >= 0.0f && CVec[i][0] <= 1.0f * mVec[i][0] * nVec[i][0]);
	}
	
	double timeOmegaTotalStart = gettime();
	
	for(unsigned int j = 0; j < iters; j++){
		avgF = 0.0f;
		maxF = 0.0f;
		minF = FLT_MAX;
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

		for(unsigned int i = 0; i <= N-4; i+=4){
			/* filling the vectors with the appropriate variable elements */
			LVec_mm = _mm_load_ps(&LVec[i][0]);
			RVec_mm = _mm_load_ps(&RVec[i][0]);
			mVec_mm = _mm_load_ps(&mVec[i][0]);
			nVec_mm = _mm_load_ps(&nVec[i][0]);
			CVec_mm = _mm_load_ps(&CVec[i][0]);
			/* 
			 * expressing operations with vector arithmetics, one instruction 
			 * at a time, where the operation num=a*(b-1)/2 becomes:
			 * sub = b-1
			 * div = sub/2
			 * num = a*div 
			 */
			
            /* Code before simultaneous executions */
			
            /* num_0 = LVec[i][0] + RVec[i][0] */
			__m128 num_0 = _mm_add_ps(LVec_mm, RVec_mm);
			/* num_1 = mVec[i][0] * (mVec[i][0] - 1.0f) / 2.0f */
			__m128 sub1 = _mm_sub_ps(mVec_mm, one_vec);
			__m128 div1 = _mm_div_ps(sub1, two_vec);
			__m128 num_1 = _mm_mul_ps(mVec_mm, div1);
			/* num_2 = nVec[i][0] * (nVec[i][0] - 1.0f) / 2.0f */
			__m128 sub2 = _mm_sub_ps(nVec_mm, one_vec);
			__m128 div2 = _mm_div_ps(sub2, two_vec);
			__m128 num_2 = _mm_mul_ps(nVec_mm, div2);
			/* num = num_0 / (num_1 + num_2) */
			__m128 add0 = _mm_add_ps(num_1, num_2);
			__m128 num = _mm_div_ps(num_0, add0);
			/* den_0 = CVec[i][0] - LVec[i][0] - RVec[i][0] */
			__m128 sub0 = _mm_sub_ps(CVec_mm, LVec_mm);
			__m128 den_0 = _mm_sub_ps(sub0, RVec_mm);
			/* den_1 = mVec[i][0] * nVec[i][0] */
			__m128 den_1 = _mm_mul_ps(mVec_mm, nVec_mm);
			/* den = den_0 / den_1 */
			__m128 den = _mm_div_ps(den_0, den_1);
			/* FVec[i][0] = num / (den + 0.01f) */
			__m128 add3 = _mm_add_ps(den, centi_vec);
			FVec_mm = _mm_div_ps(num, add3);
		
			/* maxF = FVec[i][0] > maxF ? FVec[i][0] : maxF */
			maxF_mm = _mm_max_ps(maxF_mm,FVec_mm);
			/* minF = FVec[i][0] < minF ? FVec[i][0] : minF */
			minF_mm = _mm_min_ps(minF_mm,FVec_mm);
			/* avgF += FVec[i][0] */
			avgF_mm = _mm_add_ps(avgF_mm, FVec_mm);
		}
		
		maxF = total_max(maxF_mm);
		minF = total_min(minF_mm);
		avgF = total_avg(avgF_mm);
		
		/* serial execution of residual elements (when N % 4 != 0) */
		for(int i = N-(N%4); i < N; i++){
			float num_0 = LVec[i][0] + RVec[i][0];
			float num_1 = mVec[i][0] * (mVec[i][0] - 1.0f) / 2.0f;
			float num_2 = nVec[i][0] * (nVec[i][0] - 1.0f) / 2.0f;
			float num = num_0 / (num_1 + num_2);
			float den_0 = CVec[i][0] - LVec[i][0] - RVec[i][0];
			float den_1 = mVec[i][0] * nVec[i][0];
			float den = den_0 / den_1;
			FVec[i][0] = num / (den + 0.01f);
			maxF = FVec[i][0] > maxF ? FVec[i][0] : maxF;
			minF = FVec[i][0] < minF ? FVec[i][0] : minF;
			avgF += FVec[i][0];
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
}
