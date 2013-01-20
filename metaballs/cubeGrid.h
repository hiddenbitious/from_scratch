
#ifndef _CUBEGRID_H_
#define _CUBEGRID_H_

#include "../globals.h"
#include "../bbox.h"
#include "metaball.h"
#include "../frustum.h"



#define CUBES_PER_AXIS	40
#define CUBE_SIZE		1
#define THRESHOLD		1.0f




struct grid_cube_vertex
{
	float value;
	C_Vertex position;
	C_Vertex normal; // Me ton idio tropo pou kanoume interpolation gia na broume tis nees korifes kanoume kai gia tis normes
};


struct grid_cube
{
	grid_cube_vertex *vertices[8];
};


struct grid_triangle
{
	C_Vertex vertices[3];
	C_Vertex normals[3];
};


class C_CubeGrid
{
	public:
		void Constructor ( float x , float y , float z );

		// Whole's grid position
		C_Vertex position;

		C_BBox bbox;

		// Arithmos trigonon pou sxediazondai se kathe kare
		unsigned long nTriangles;


		// Pinakas pou tha krataei tous kibous apo tous opoious tha apoteleite to grid
		grid_cube gridCubes[ CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS ];


		// Oi korifes metaksi ton kibon tha moirazondai
		grid_cube_vertex gridCubeVertices [ (CUBES_PER_AXIS+1)*(CUBES_PER_AXIS+1)*(CUBES_PER_AXIS+1) ];


		unsigned long nGridCubes;
		unsigned long nGridCubeVertices;


		grid_triangle geometry[40000];

		void Update ( C_Metaball *metaballs , unsigned short nBalls , C_Frustum *frustum );
		int Draw ( C_Frustum *frustum );
		void DrawGridCube ( void );
};

#endif
