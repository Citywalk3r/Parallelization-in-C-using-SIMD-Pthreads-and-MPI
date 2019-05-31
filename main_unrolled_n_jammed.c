#include "generic.h"

int main(int argc, char ** argv){
	assert(argc == 2);

	double timeTotalMainStart = gettime();

	float avgF = 0.0f;
	float maxF = 0.0f;
	float minF = FLT_MAX;

	unsigned int N = (unsigned int)atoi(argv[1]);

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

	double timeOmegaTotalStart = gettime();
	for(unsigned int j = 0; j < iters; j++){
		avgF = 0.0f;
		maxF = 0.0f;
		minF = FLT_MAX;
		
		/* Jamming the unrolled version to measure performance with jamming */
		for(unsigned int i = 0; i <= N-4; i+=4){
            float num_0[4], num_1[4], num_2[4], num[4], den_0[4], den_1[4], den[4];
            
			num_0[0] = LVec[i] + RVec[i];
            num_0[1] = LVec[i+1] + RVec[i+1];
            num_0[2] = LVec[i+2] + RVec[i+2];
            num_0[3] = LVec[i+3] + RVec[i+3];
            
			num_1[0] = mVec[i] * (mVec[i] - 1.0f) / 2.0f;
			num_1[1] = mVec[i+1] * (mVec[i+1] - 1.0f) / 2.0f;
            num_1[2] = mVec[i+2] * (mVec[i+2] - 1.0f) / 2.0f;
            num_1[3] = mVec[i+3] * (mVec[i+3] - 1.0f) / 2.0f;
            
            num_2[0] = nVec[i] * (nVec[i] - 1.0f) / 2.0f;
            num_2[1] = nVec[i+1] * (nVec[i+1] - 1.0f) / 2.0f;
            num_2[2] = nVec[i+2] * (nVec[i+2] - 1.0f) / 2.0f;
            num_2[3] = nVec[i+3] * (nVec[i+3] - 1.0f) / 2.0f;
            
			num[0] = num_0[0] / (num_1[0] + num_2[0]);
            num[1] = num_0[1] / (num_1[1] + num_2[1]);
            num[2] = num_0[2] / (num_1[2] + num_2[2]);
            num[3] = num_0[3] / (num_1[3] + num_2[3]);
            
			den_0[0] = CVec[i] - LVec[i] - RVec[i];
            den_0[1] = CVec[i+1] - LVec[i+1] - RVec[i+1];
            den_0[2] = CVec[i+2] - LVec[i+2] - RVec[i+2];
            den_0[3] = CVec[i+3] - LVec[i+3] - RVec[i+3];
            
			den_1[0] = mVec[i] * nVec[i];
            den_1[1] = mVec[i+1] * nVec[i+1];
            den_1[2] = mVec[i+2] * nVec[i+2];
            den_1[3] = mVec[i+3] * nVec[i+3];
            
			den[0] = den_0[0] / den_1[0];
            den[1] = den_0[1] / den_1[1];
            den[2] = den_0[2] / den_1[2];
            den[3] = den_0[3] / den_1[3];
            
			FVec[i] = num[0] / (den[0] + 0.01f);
            FVec[i+1] = num[1] / (den[1] + 0.01f);
            FVec[i+2] = num[2] / (den[2] + 0.01f);
            FVec[i+3] = num[3] / (den[3] + 0.01f);
            
			maxF = FVec[i] > maxF ? FVec[i] : maxF;
            maxF = FVec[i+1] > maxF ? FVec[i+1] : maxF;
            maxF = FVec[i+2] > maxF ? FVec[i+2] : maxF;
            maxF = FVec[i+3] > maxF ? FVec[i+3] : maxF;
            
			minF = FVec[i] < minF ? FVec[i] : minF;
            minF = FVec[i+1] < minF ? FVec[i+1] : minF;
            minF = FVec[i+2] < minF ? FVec[i+2] : minF;
            minF = FVec[i+3] < minF ? FVec[i+3] : minF;
            
			avgF += FVec[i];
            avgF += FVec[i+1];
            avgF += FVec[i+2];
            avgF += FVec[i+3];
		}
		
		for(unsigned int i = N-(N%4); i < N; i++){
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
}
