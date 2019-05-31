/*
 * Functions used by SSE
 */
#ifndef GLOBAL_H
#include "generic.h"
#endif

#ifndef SSE_H
#define SSE_H
#include <xmmintrin.h>
#include <pmmintrin.h>
/* Helper function to print __m128 values */
void print_ps(char* name, __m128 var);
/* Function to calculate max (float) in a __m128 value */
float total_max(__m128 new_mm);
/* Function to calculate min (float) in a __m128 value */
float total_min( __m128 new_mm);
/* Function to calculate avg (float) in a __m128 value */
float total_avg(__m128 new_mm);
#endif
