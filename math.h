/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

#ifndef _MATH_H_
#define _MATH_H_

#include "globals.h"
#include <limits>
#include <math.h>

#define PI 3.1415926535897932384626433832795f
#define DEGREES_TO_RADIANS 0.0174532925199432957692369076848861f
#define RADIANS_TO_DEGREES 57.2957795130823208767981548141052f

#define MAX(a,b) (( a > b ) ? a : b)
#define MIN(a,b) (( a < b ) ? a : b)

#define EPSILON 1.0e-4f
#define FLOAT_EQ(x,v) (((v - EPSILON)<=x) && (x <=( v + EPSILON)))

/// Numeric limts
#define FLT_MAX numeric_limits<float>::max()
#define FLT_MIN numeric_limits<float>::min()
#define INT_MAX numeric_limits<int>::min()
#define INT_MIN numeric_limits<int>::min()

#define SMALLEST_FLOAT -1*( FLT_MAX - 1 )
#define GREATEST_FLOAT FLT_MAX

/// limits a value to low and high
#define LIMIT_RANGE(low, value, high)	{	if (value < low)	value = low;	else if(value > high)	value = high;	}
#define UPPER_LIMIT (value , limit )		{ if ( value > limit ) value = limit; }
#define ABS(value)		{	 if ( value < 0 )	value *= -1.0;	}

namespace math {
void Normalize(float* x , float* y , float* z);
float Magnitude(float x , float y , float z);
float PointToPointDistance(C_Vertex *p1 , C_Vertex *p2);
}

#endif
