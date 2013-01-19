
#include "cubeGrid.h"
#include "tables.h"
//#include <gl\gl.h>
#include <gl\glut.h>
#include "../math.h"



struct grid_vertex
{
	C_Vertex vertex;
	C_Vertex normal;
};


inline float fieldFormula ( float q , float r )
{
	return (q / r*5);
}

void C_CubeGrid::Constructor ( float x , float y , float z )
{
	position.x = x;
	position.y = y;
	position.z = z;

	nGridCubes = CUBES_PER_AXIS * CUBES_PER_AXIS * CUBES_PER_AXIS;
	nGridCubeVertices = (CUBES_PER_AXIS+1) * (CUBES_PER_AXIS+1) * (CUBES_PER_AXIS+1);
	nTriangles = 0;

	unsigned long cc = 0;

	// Arhikopoiisi ton korifon ton kibon
	for ( int x = 0 ; x < CUBES_PER_AXIS+1 ; x++ )
	{
		for ( int y = 0 ; y < CUBES_PER_AXIS+1 ; y++ )
		{
			for ( int z = 0 ; z < CUBES_PER_AXIS+1 ; z++ )
			{
				gridCubeVertices[cc].position.x = x * CUBE_SIZE;
				gridCubeVertices[cc].position.y = y * CUBE_SIZE;
				gridCubeVertices[cc].position.z = z * CUBE_SIZE;

				gridCubeVertices[cc].value = 0.0f;

				++cc;
			}
		}
	}

	// Sindesi ton kibon me tis korifes tous
	cc = 0;

	for ( int x = 0 ; x < CUBES_PER_AXIS ; x++ )
	{
		for ( int y = 0 ; y < CUBES_PER_AXIS ; y++ )
		{
			for ( int z = 0 ; z < CUBES_PER_AXIS ; z++ )
			{
				gridCubes[cc].vertices[0] = &gridCubeVertices[(x*(CUBES_PER_AXIS+1)+y)*(CUBES_PER_AXIS+1)+z];
				gridCubes[cc].vertices[1] = &gridCubeVertices[(x*(CUBES_PER_AXIS+1)+y)*(CUBES_PER_AXIS+1)+z+1];
				gridCubes[cc].vertices[2] = &gridCubeVertices[(x*(CUBES_PER_AXIS+1)+(y+1))*(CUBES_PER_AXIS+1)+z+1];
				gridCubes[cc].vertices[3] = &gridCubeVertices[(x*(CUBES_PER_AXIS+1)+(y+1))*(CUBES_PER_AXIS+1)+z];

				gridCubes[cc].vertices[4] = &gridCubeVertices[((x+1)*(CUBES_PER_AXIS+1)+y)*(CUBES_PER_AXIS+1)+z];
				gridCubes[cc].vertices[5] = &gridCubeVertices[((x+1)*(CUBES_PER_AXIS+1)+y)*(CUBES_PER_AXIS+1)+z+1];
				gridCubes[cc].vertices[6] = &gridCubeVertices[((x+1)*(CUBES_PER_AXIS+1)+(y+1))*(CUBES_PER_AXIS+1)+z+1];
				gridCubes[cc].vertices[7] = &gridCubeVertices[((x+1)*(CUBES_PER_AXIS+1)+(y+1))*(CUBES_PER_AXIS+1)+z];

				++cc;
			}
		}
	}

	bbox.SetMin ( position.x , position.y , position.z );
	bbox.SetMax ( position.x + CUBES_PER_AXIS * CUBE_SIZE ,
				  position.y + CUBES_PER_AXIS * CUBE_SIZE ,
				  position.z + CUBES_PER_AXIS * CUBE_SIZE );
	bbox.SetVertices ();
}


void C_CubeGrid::Update ( C_Metaball *metaballs , unsigned short nBalls , C_Frustum *frustum )
{
	if ( frustum != NULL )
	{
		if ( !frustum->cubeInFrustum ( &bbox ) )
			return;
	}

	float rad , dist , normalScale;
	C_Vertex pos;
	C_Vertex ballToPoint;


	// Katharise ta palia values kai normes
	for ( int i = 0 ; i < nGridCubeVertices ; i++ )
	{
		gridCubeVertices[i].value = 0.0f;
		gridCubeVertices[i].normal.x = gridCubeVertices[i].normal.y = gridCubeVertices[i].normal.z = 0.0f;
	}
	//memset ( gridCubeVertices , 0 , nGridCubeVertices * sizeof ( grid_cube_vertex ) );


	// Midenismos metriti
	nTriangles = 0;



	// For each metaball
	for ( int cb = 0 ; cb < nBalls ; cb++ )
	{
		rad = metaballs[cb].radius;
		pos.x = metaballs[cb].position.x;pos.y = metaballs[cb].position.y;pos.z = metaballs[cb].position.z;

		// For each gridCubeVertex
		for ( int cv = 0 ; cv < nGridCubeVertices ; cv++ )
		{
			ballToPoint.x = gridCubeVertices[cv].position.x - metaballs[cb].position.x;
			ballToPoint.y = gridCubeVertices[cv].position.y - metaballs[cb].position.y;
			ballToPoint.z = gridCubeVertices[cv].position.z - metaballs[cb].position.z;

			dist = ballToPoint.x*ballToPoint.x + ballToPoint.y*ballToPoint.y + ballToPoint.z*ballToPoint.z;

			if ( dist <= 0.0001f )
				dist = 0.0001f;

			gridCubeVertices[cv].value += fieldFormula ( rad , dist );

			//normal = (r^2 * v)/d^4
			normalScale = rad / (dist*dist);

			gridCubeVertices[cv].normal.x += ballToPoint.x * normalScale;
			gridCubeVertices[cv].normal.y += ballToPoint.y * normalScale;
			gridCubeVertices[cv].normal.z += ballToPoint.z * normalScale;
		}
	}

	// Kanonikopoisi normon.... Poli akribo etsi opos einai...
	for ( int cv = 0 ; cv < nGridCubeVertices ; cv++ )
		math::Normalize ( &gridCubeVertices[cv].normal.x , &gridCubeVertices[cv].normal.y , &gridCubeVertices[cv].normal.z );



	// Ipologismos poligonon
	grid_vertex edgeVertices[12];

	// Gia kathe kibo...
	for ( int cb = 0 ; cb < nGridCubes ; cb++ )
	{
		unsigned char cubeIndex = 0x00;

		if ( gridCubes[cb].vertices[0]->value < THRESHOLD )
			cubeIndex |= 1;
		if ( gridCubes[cb].vertices[1]->value < THRESHOLD )
			cubeIndex |= 2;
		if ( gridCubes[cb].vertices[2]->value < THRESHOLD )
			cubeIndex |= 4;
		if ( gridCubes[cb].vertices[3]->value < THRESHOLD )
			cubeIndex |= 8;
		if ( gridCubes[cb].vertices[4]->value < THRESHOLD )
			cubeIndex |= 16;
		if ( gridCubes[cb].vertices[5]->value < THRESHOLD )
			cubeIndex |= 32;
		if ( gridCubes[cb].vertices[6]->value < THRESHOLD )
			cubeIndex |= 64;
		if ( gridCubes[cb].vertices[7]->value < THRESHOLD )
			cubeIndex |= 128;
		
		// ...bres poies pleures einai entos tou paidiou
		short usedEdges = edgeTable[cubeIndex];

		//if the cube is entirely within/outside surface, no faces
		if( usedEdges == 0 || usedEdges == 255 )
			continue;

		for ( int currentEdge = 0 ; currentEdge < 12 ; currentEdge++ )
		{
			if ( usedEdges && 1<<currentEdge )
			{
				grid_cube_vertex *v1 = gridCubes[cb].vertices[verticesAtEndsOfEdges[currentEdge*2  ]];
				grid_cube_vertex *v2 = gridCubes[cb].vertices[verticesAtEndsOfEdges[currentEdge*2+1]];

				float delta = ( THRESHOLD - v1->value ) / ( v2->value - v1->value );

				edgeVertices[currentEdge].vertex.x = v1->position.x + delta*(v2->position.x - v1->position.x);
				edgeVertices[currentEdge].vertex.y = v1->position.y + delta*(v2->position.y - v1->position.y);
				edgeVertices[currentEdge].vertex.z = v1->position.z + delta*(v2->position.z - v1->position.z);

				edgeVertices[currentEdge].normal.x = v1->normal.x + delta * (v2->normal.x - v1->normal.x);
				edgeVertices[currentEdge].normal.y = v1->normal.y + delta * (v2->normal.y - v1->normal.y);
				edgeVertices[currentEdge].normal.z = v1->normal.z + delta * (v2->normal.z - v1->normal.z);
			}
		}


		for ( int k = 0 ; triTable[cubeIndex][k] != -1 ; k += 3 )
		{
			geometry[nTriangles].vertices[0].x = edgeVertices[triTable[cubeIndex][k  ]].vertex.x;
			geometry[nTriangles].vertices[0].y = edgeVertices[triTable[cubeIndex][k  ]].vertex.y;
			geometry[nTriangles].vertices[0].z = edgeVertices[triTable[cubeIndex][k  ]].vertex.z;

			geometry[nTriangles].normals[0].x = edgeVertices[triTable[cubeIndex][k  ]].normal.x;
			geometry[nTriangles].normals[0].y = edgeVertices[triTable[cubeIndex][k  ]].normal.y;
			geometry[nTriangles].normals[0].z = edgeVertices[triTable[cubeIndex][k  ]].normal.z;

			//

			geometry[nTriangles].vertices[1].x = edgeVertices[triTable[cubeIndex][k+2]].vertex.x;
			geometry[nTriangles].vertices[1].y = edgeVertices[triTable[cubeIndex][k+2]].vertex.y;
			geometry[nTriangles].vertices[1].z = edgeVertices[triTable[cubeIndex][k+2]].vertex.z;

			geometry[nTriangles].normals[1].x = edgeVertices[triTable[cubeIndex][k+2]].normal.x;
			geometry[nTriangles].normals[1].y = edgeVertices[triTable[cubeIndex][k+2]].normal.y;
			geometry[nTriangles].normals[1].z = edgeVertices[triTable[cubeIndex][k+2]].normal.z;

			//

			geometry[nTriangles].vertices[2].x = edgeVertices[triTable[cubeIndex][k+1]].vertex.x;
			geometry[nTriangles].vertices[2].y = edgeVertices[triTable[cubeIndex][k+1]].vertex.y;
			geometry[nTriangles].vertices[2].z = edgeVertices[triTable[cubeIndex][k+1]].vertex.z;

			geometry[nTriangles].normals[2].x = edgeVertices[triTable[cubeIndex][k+1]].normal.x;
			geometry[nTriangles].normals[2].y = edgeVertices[triTable[cubeIndex][k+1]].normal.y;
			geometry[nTriangles].normals[2].z = edgeVertices[triTable[cubeIndex][k+1]].normal.z;

			nTriangles++;
		}
	}
}


int C_CubeGrid::Draw ( C_Frustum *frustum )
{
	if ( frustum != NULL )
	{
		if ( frustum->cubeInFrustum ( &bbox ) == false )
			return 0;
	}

	glPushMatrix ();

	glTranslatef ( position.x , position.y , position.z );

	glColor3f ( 1.0f , 1.0f , 1.0f );

	glBegin ( GL_TRIANGLES );
		for ( int i = 0 ; i < nTriangles ; i++ )
		{
			glNormal3f ( geometry[i].normals[0].x , geometry[i].normals[0].y , geometry[i].normals[0].z );
			glVertex3f ( geometry[i].vertices[0].x , geometry[i].vertices[0].y , geometry[i].vertices[0].z );

			glNormal3f ( geometry[i].normals[1].x , geometry[i].normals[1].y , geometry[i].normals[1].z );
			glVertex3f ( geometry[i].vertices[1].x , geometry[i].vertices[1].y , geometry[i].vertices[1].z );

			glNormal3f ( geometry[i].normals[2].x , geometry[i].normals[2].y , geometry[i].normals[2].z );
			glVertex3f ( geometry[i].vertices[2].x , geometry[i].vertices[2].y , geometry[i].vertices[2].z );
		}
	glEnd ();

	glPopMatrix ();

	return nTriangles;
}


void C_CubeGrid::DrawGridCube ( void )
{
/*
	glColor3f ( 1.0f , 1.0f , 1.0f );

	glDisable ( GL_LIGHTING );
		glPushMatrix ();
			glTranslatef ( CUBES_PER_AXIS * CUBE_SIZE / 2 , CUBES_PER_AXIS * CUBE_SIZE / 2 , CUBES_PER_AXIS * CUBE_SIZE / 2 );
			glutWireCube ( CUBES_PER_AXIS * CUBE_SIZE );
		glPopMatrix ();
	glEnable ( GL_LIGHTING );
*/
	GLboolean culling;
	glGetBooleanv ( GL_CULL_FACE , &culling );

	if ( culling )
		glDisable ( GL_CULL_FACE );

	bbox.Draw ();

	if ( culling )
		glEnable ( GL_CULL_FACE );
}