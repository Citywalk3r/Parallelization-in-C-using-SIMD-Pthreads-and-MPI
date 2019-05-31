/*
 * Functions used by all implementations 
 */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <float.h>

#include <string.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <stdbool.h>

#define MINSNPS_B 5
#define MAXSNPS_E 20

double gettime(void);
float randpval (void);

#endif
