#include "generic.h"

double gettime(void) {
	struct timeval ttime;
	gettimeofday(&ttime, NULL);
	return ttime.tv_sec + ttime.tv_usec * 0.000001;
}

float randpval (void) {
	int vr = rand();
	int vm = rand() % vr;
	float r = ((float)vm)/(float)vr;
	assert(r>=0.0f && r<=1.00001f);
	return r;
}

