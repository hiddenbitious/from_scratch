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

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef unsigned short USHORT;

//Very simple struct to hold vertex information
typedef struct
{
	float x , y , z;
} C_Vertex;

//Very simple struct to hold normal information
typedef struct
{
	float nx , ny , nz;
} C_Normal;

//Very simple struct to hold texture coordinate information
typedef struct
{
	float u , v;
} C_TexCoord;

//Very simple struct to hold color information
typedef struct
{
	float r , g , b , a;
} C_Color;

//Very simple struct to hold index information
typedef struct
{
	UINT p0 , p1 , p2;
} C_TriIndices;

class C_3DSReader;
class C_Camera;
class C_Quaternion;
class vector2;
class C_Vector3;

#endif
