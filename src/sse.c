#include "sse.h"

void print_ps(char* name, __m128 var){
	float *val = (float*) &var;
	printf("Values of %s: %f %f %f %f\n", name,val[0],val[1],val[2],val[3]);
}

/* 
 * Below are 3 of the many different ways to calculate the totals.
 * Using either is fine, just messing here...
 * 
 */

float total_max(__m128 new_mm){
	float *new = (float*) &new_mm;
	float max = new[0];

	max = new[0] > max ? new[0] : max;
	max = new[1] > max ? new[1] : max;
	max = new[2] > max ? new[2] : max;
	max = new[3] > max ? new[3] : max;
	
	return max;
}

float total_min( __m128 new_mm){
	float *new = (float*) &new_mm;
	float min;
	
	min = new[0] < new[1] ? new[0] : new[1];
	min = new[2] < min ? new[2] : min;
	min = new[3] < min ? new[3] : min;
	
	return min;
}

float total_avg(__m128 new_mm){
	
	new_mm = _mm_hadd_ps(new_mm, new_mm);
	new_mm = _mm_hadd_ps(new_mm, new_mm);
	/* Intel guys total morons for choice of words in _mm_extract_ps... */
	return _mm_cvtss_f32(new_mm);
	
	/* 
	 * Alternatively, we could use the same way as max/min, but learning a way 
	 * to do it with SIMD is nice :D
	 */
// 	float *new = (float*) &new_mm;
// 	float avg = 0.0f;
// 	for (i = 0; i < 4; i++)
// 		avg += new[i];
// 	return avg;
}
