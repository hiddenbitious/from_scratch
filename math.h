#ifndef _MATH_H_
#define _MATH_H_

#include "globals.h"
#include <limits>
#include <math.h>

#define PI                    3.1415926535897932384626433832795f
#define DEGREES_TO_RADIANS    0.0174532925199432957692369076848861f
#define RADIANS_TO_DEGREES    57.2957795130823208767981548141052f

#define MAX(a,b)              (( a > b ) ? a : b)
#define MIN(a,b)              (( a < b ) ? a : b)

#define EPSILON               1.0e-4f
//#define EPSILON               std::numeric_limits<float>::epsilon()
//#define FLOAT_EQ(a,b)         ((((b) - EPSILON) <= (a)) && (((b) + EPSILON)) >= (a))
#define FLOAT_EQ(a, b)        (fabs((a) - (b)) < EPSILON)
#define FLOAT_GREATER(a, b)   (!(FLOAT_EQ((a), (b))) && (a) > (b))
#define FLOAT_SMALLER(a, b)   (!(FLOAT_EQ((a), (b))) && (a) < (b))

/// Numeric limts
#define FLT_MAX numeric_limits<float>::max()
#define FLT_MIN numeric_limits<float>::min()
#define INT_MAX numeric_limits<int>::max()
#define INT_MIN numeric_limits<int>::min()
#define UINT_MAX numeric_limits<unsigned int>::min()
#define UINT_MIN numeric_limits<unsigned int>::min()

#define SMALLEST_FLOAT -FLT_MAX
#define GREATEST_FLOAT FLT_MAX

/// limits a value to low and high
#define LIMIT_RANGE(low, value, high)	{ if (value < low) value = low; else if(value > high) value = high; }
#define UPPER_LIMIT (value , limit )	{ if ( value > limit ) value = limit; }
#define ABS(value)		               { if ( value < 0 ) value *= -1.0; }

namespace math {
   void Normalize(float* x , float* y , float* z);
   void Normalize(C_Vertex *vec);
   float Magnitude(float x , float y , float z);
   float PointToPointDistance(C_Vertex *p1 , C_Vertex *p2);
   C_Vertex transformPoint(const ESMatrix *matrix, const C_Vertex *point);
   C_Vector3 transformPoint(const ESMatrix *matrix, const C_Vector3 *point);
   C_Vertex CrossProduct(const C_Vertex* v1 , const C_Vertex* v2 , const C_Vertex* v3);
   C_Vertex CrossProduct(const C_Vertex* v1 , const C_Vertex* v2);
}

extern const ESMatrix Identity;

/// Code ripped from "OpenGL ES 2.0 Programming Guide"
void esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz);
void esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz);
void esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
void esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ);
void esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ);
void esMatrixMultiply(ESMatrix *result, const ESMatrix *srcA, const ESMatrix *srcB);
void esMatrixLoadIdentity(ESMatrix *result);

#endif
