#include <string.h>
#include <stdio.h>

#include "math.h"
#include "vectors.h"

const ESMatrix Identity = {{{1.0f, 0.0f, 0.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f, 0.0f},
                            {0.0f, 0.0f, 0.0f, 1.0f}}};

float
math::Magnitude(float x , float y , float z)
{
   return (sqrtf(x * x + y * y + z * z));
}

void
math::Normalize(C_Vertex *vec)
{
	float lenght = math::Magnitude(vec->x, vec->y, vec->z);

	if(lenght) {
		vec->x /= lenght;
		vec->y /= lenght;
		vec->z /= lenght;
   }
}

C_Vertex
math::CrossProduct(const C_Vertex* v1 , const C_Vertex* v2 , const C_Vertex* v3)
{
	C_Vertex tempVec;

	tempVec.x = (v1->y - v2->y) * (v2->z - v3->z)  - (v2->y - v3->y) * (v1->z - v2->z);
	tempVec.y = (v2->x - v3->x) * (v1->z - v2->z)  - (v1->x - v2->x) * (v2->z - v3->z);
	tempVec.z = (v1->x - v2->x) * (v2->y - v3->y)  - (v2->x - v3->x) * (v1->y - v2->y);

	return tempVec;
}

C_Vertex
math::CrossProduct(const C_Vertex* v1, const C_Vertex* v2)
{
	C_Vertex tempVec;

	tempVec.x = v1->y * v2->z - v1->z * v2->y;
	tempVec.y = v1->z * v2->x - v1->x * v2->z;
	tempVec.z = v1->x * v2->y - v1->y * v2->x;

	return tempVec;
}

void
math::Normalize(float* x , float* y , float *z)
{
   float length = math::Magnitude(*x , *y , *z);

   if(length) {
      *x /= length;
      *y /= length;
      *z /= length;
   }
}

float
math::Distance(C_Vertex *p1 , C_Vertex *p2)
{
   return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y) + (p2->z - p1->z) * (p2->z - p1->z));
}

C_Vertex
math::transformPoint(const ESMatrix *matrix, const C_Vertex *point)
{
   C_Vertex transformedPoint;

   transformedPoint.x = point->x * matrix->m[0][0] + point->y * matrix->m[1][0] + point->z * matrix->m[2][0] + matrix->m[3][0];
   transformedPoint.y = point->x * matrix->m[0][1] + point->y * matrix->m[1][1] + point->z * matrix->m[2][1] + matrix->m[3][1];
   transformedPoint.z = point->x * matrix->m[0][2] + point->y * matrix->m[1][2] + point->z * matrix->m[2][2] + matrix->m[3][2];
//   transformedPoint.w = point->x * matrix->m[0][3] + point->y * matrix->m[1][3] + point->z * matrix->m[2][3] + matrix->m[3][3];

   return transformedPoint;
}

C_Vertex
math::transformNormal(const ESMatrix *matrix, const C_Vertex *point)
{
   C_Vertex transformedPoint;

   transformedPoint.x = point->x * matrix->m[0][0] + point->y * matrix->m[1][0] + point->z * matrix->m[2][0];
   transformedPoint.y = point->x * matrix->m[0][1] + point->y * matrix->m[1][1] + point->z * matrix->m[2][1];
   transformedPoint.z = point->x * matrix->m[0][2] + point->y * matrix->m[1][2] + point->z * matrix->m[2][2];

   return transformedPoint;
}

C_Vector3
math::transformPoint(const ESMatrix *matrix, const C_Vector3 *point)
{
   C_Vector3 transformedPoint;

   transformedPoint.x = point->x * matrix->m[0][0] + point->y * matrix->m[1][0] + point->z * matrix->m[2][0] + matrix->m[3][0];
   transformedPoint.y = point->x * matrix->m[0][1] + point->y * matrix->m[1][1] + point->z * matrix->m[2][1] + matrix->m[3][1];
   transformedPoint.z = point->x * matrix->m[0][2] + point->y * matrix->m[1][2] + point->z * matrix->m[2][2] + matrix->m[3][2];
//   transformedPoint.w = point->x * matrix->m[0][3] + point->y * matrix->m[1][3] + point->z * matrix->m[2][3] + matrix->m[3][3];

   return transformedPoint;
}

/// **************************************************************
/// Book:      OpenGL(R) ES 2.0 Programming Guide
/// Authors:   Aaftab Munshi, Dan Ginsburg, Dave Shreiner
/// ISBN-10:   0321502795
/// ISBN-13:   9780321502797
/// Publisher: Addison-Wesley Professional
/// URLs:      http://safari.informit.com/9780321563835
///            http://www.opengles-book.com
///

/// ESUtil.c
///
///    A utility library for OpenGL ES.  This library provides a
///    basic common framework for the example applications in the
///    OpenGL ES 2.0 Programming Guide.
///
/// **************************************************************
void
esScale(ESMatrix *result, GLfloat sx, GLfloat sy, GLfloat sz)
{
   result->m[0][0] *= sx;
   result->m[0][1] *= sx;
   result->m[0][2] *= sx;
   result->m[0][3] *= sx;

   result->m[1][0] *= sy;
   result->m[1][1] *= sy;
   result->m[1][2] *= sy;
   result->m[1][3] *= sy;

   result->m[2][0] *= sz;
   result->m[2][1] *= sz;
   result->m[2][2] *= sz;
   result->m[2][3] *= sz;
}

void
esTranslate(ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz)
{
   result->m[3][0] += (result->m[0][0] * tx + result->m[1][0] * ty + result->m[2][0] * tz);
   result->m[3][1] += (result->m[0][1] * tx + result->m[1][1] * ty + result->m[2][1] * tz);
   result->m[3][2] += (result->m[0][2] * tx + result->m[1][2] * ty + result->m[2][2] * tz);
   result->m[3][3] += (result->m[0][3] * tx + result->m[1][3] * ty + result->m[2][3] * tz);
}

void
esRotate(ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
   GLfloat sinAngle, cosAngle;
   GLfloat mag = sqrtf(x * x + y * y + z * z);

   sinAngle = sinf ( angle * PI / 180.0f );
   cosAngle = cosf ( angle * PI / 180.0f );
   if ( mag > 0.0f ) {
      GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs;
      GLfloat oneMinusCos;
      ESMatrix rotMat;

      x /= mag;
      y /= mag;
      z /= mag;

      xx = x * x;
      yy = y * y;
      zz = z * z;
      xy = x * y;
      yz = y * z;
      zx = z * x;
      xs = x * sinAngle;
      ys = y * sinAngle;
      zs = z * sinAngle;
      oneMinusCos = 1.0f - cosAngle;

      rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
      rotMat.m[0][1] = (oneMinusCos * xy) - zs;
      rotMat.m[0][2] = (oneMinusCos * zx) + ys;
      rotMat.m[0][3] = 0.0F;

      rotMat.m[1][0] = (oneMinusCos * xy) + zs;
      rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
      rotMat.m[1][2] = (oneMinusCos * yz) - xs;
      rotMat.m[1][3] = 0.0F;

      rotMat.m[2][0] = (oneMinusCos * zx) - ys;
      rotMat.m[2][1] = (oneMinusCos * yz) + xs;
      rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
      rotMat.m[2][3] = 0.0F;

      rotMat.m[3][0] = 0.0F;
      rotMat.m[3][1] = 0.0F;
      rotMat.m[3][2] = 0.0F;
      rotMat.m[3][3] = 1.0F;

      esMatrixMultiply( result, &rotMat, result );
   } else
      assert(0);
}

void
esFrustum(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
   float       deltaX = right - left;
   float       deltaY = top - bottom;
   float       deltaZ = farZ - nearZ;
   ESMatrix    frust;

   if((nearZ <= 0.0f) || (farZ <= 0.0f) || (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f))
      return;

   frust.m[0][0] = 2.0f * nearZ / deltaX;
   frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;

   frust.m[1][1] = 2.0f * nearZ / deltaY;
   frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;

   frust.m[2][0] = (right + left) / deltaX;
   frust.m[2][1] = (top + bottom) / deltaY;
   frust.m[2][2] = -(nearZ + farZ) / deltaZ;
   frust.m[2][3] = -1.0f;

   frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
   frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;

   esMatrixMultiply(result, &frust, result);
}

void
esPerspective(ESMatrix *result, float fovy, float aspect, float nearZ, float farZ)
{
   GLfloat frustumW, frustumH;

   frustumH = tanf( fovy / 360.0f * PI ) * nearZ;
   frustumW = frustumH * aspect;

   esFrustum( result, -frustumW, frustumW, -frustumH, frustumH, nearZ, farZ );
}

void
esOrtho(ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ)
{
   float       deltaX = right - left;
   float       deltaY = top - bottom;
   float       deltaZ = farZ - nearZ;
   ESMatrix    ortho;

   if ( (deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f) )
      return;

   esMatrixLoadIdentity(&ortho);
   ortho.m[0][0] = 2.0f / deltaX;
   ortho.m[3][0] = -(right + left) / deltaX;
   ortho.m[1][1] = 2.0f / deltaY;
   ortho.m[3][1] = -(top + bottom) / deltaY;
   ortho.m[2][2] = -2.0f / deltaZ;
   ortho.m[3][2] = -(nearZ + farZ) / deltaZ;

   esMatrixMultiply(result, &ortho, result);
}

void
esMatrixMultiply(ESMatrix *result, const ESMatrix *srcA, const ESMatrix *srcB)
{
   int i;

   if(result != srcA && result != srcB) {
     for (i=0; i<4; i++) {
         result->m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
                        (srcA->m[i][1] * srcB->m[1][0]) +
                        (srcA->m[i][2] * srcB->m[2][0]) +
                        (srcA->m[i][3] * srcB->m[3][0]) ;

         result->m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) +
                        (srcA->m[i][1] * srcB->m[1][1]) +
                        (srcA->m[i][2] * srcB->m[2][1]) +
                        (srcA->m[i][3] * srcB->m[3][1]) ;

         result->m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) +
                        (srcA->m[i][1] * srcB->m[1][2]) +
                        (srcA->m[i][2] * srcB->m[2][2]) +
                        (srcA->m[i][3] * srcB->m[3][2]) ;

         result->m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) +
                        (srcA->m[i][1] * srcB->m[1][3]) +
                        (srcA->m[i][2] * srcB->m[2][3]) +
                        (srcA->m[i][3] * srcB->m[3][3]) ;
      }
   } else {
      ESMatrix    tmp;
      for (i=0; i<4; i++) {
         tmp.m[i][0] =	(srcA->m[i][0] * srcB->m[0][0]) +
                        (srcA->m[i][1] * srcB->m[1][0]) +
                        (srcA->m[i][2] * srcB->m[2][0]) +
                        (srcA->m[i][3] * srcB->m[3][0]) ;

         tmp.m[i][1] =	(srcA->m[i][0] * srcB->m[0][1]) +
                        (srcA->m[i][1] * srcB->m[1][1]) +
                        (srcA->m[i][2] * srcB->m[2][1]) +
                        (srcA->m[i][3] * srcB->m[3][1]) ;

         tmp.m[i][2] =	(srcA->m[i][0] * srcB->m[0][2]) +
                        (srcA->m[i][1] * srcB->m[1][2]) +
                        (srcA->m[i][2] * srcB->m[2][2]) +
                        (srcA->m[i][3] * srcB->m[3][2]) ;

         tmp.m[i][3] =	(srcA->m[i][0] * srcB->m[0][3]) +
                        (srcA->m[i][1] * srcB->m[1][3]) +
                        (srcA->m[i][2] * srcB->m[2][3]) +
                        (srcA->m[i][3] * srcB->m[3][3]) ;
      }
      memcpy(result, &tmp, sizeof(ESMatrix));
   }
}

void
esMatrixLoadIdentity(ESMatrix *result)
{
   memcpy(result, (void *)&Identity, sizeof(ESMatrix));
}
