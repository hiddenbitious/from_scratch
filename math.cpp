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

#include "math.h"

float math::Magnitude(float x , float y , float z)
{
	return (sqrt(x * x + y * y + z * z));
}


void math::Normalize(float* x , float* y , float *z)
{
	float length = math::Magnitude(*x , *y , *z);

	if(length) {
		*x /= length;
		*y /= length;
		*z /= length;
	}
}


float math::PointToPointDistance(C_Vertex *p1 , C_Vertex *p2)
{
	return sqrt((p2->x - p1->x) * (p2->x - p1->x) + (p2->y - p1->y) * (p2->y - p1->y) + (p2->z - p1->z) * (p2->z - p1->z));
}
