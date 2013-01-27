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

#include "mesh.h"
#include <memory.h>
#include <iostream>
using namespace std;


C_Mesh::C_Mesh(void)
{
	nVertices	= 0;
	nPolys		= 0;
	nReferences = 0;

	vertices	= NULL;
	indices		= NULL;
	normals		= NULL;
	texCoords	= NULL;
	color		= NULL;
}


void C_Mesh::UnRef(void)
{
	nReferences--;

	if(nReferences <= 0) {
		if(vertices) {
			delete vertices;
			vertices = NULL;
		}
		if(indices) {
			delete indices;
			indices = NULL;
		}
		if(normals) {
			delete normals;
			normals = NULL;
		}
		if(texCoords) {
			delete texCoords;
			texCoords = NULL;
		}
		if(color) {
			delete color;
			color = NULL;
		}

		nPolys = 0;
		nVertices = 0;
		nReferences = 0;
	}
}


void C_Mesh::Ref(void)
{
	nReferences++;
}


void C_Mesh::CalcNormals(bool invertNormals)
{
	if(vertices == NULL) { return; }
	if(indices == NULL) { return; }

	if(normals != NULL) { delete normals; }

	C_Vector3 tempVec;
	ULONG i;

	normals = new C_Normal[nVertices];
	USHORT* connections = new USHORT[nVertices];

	memset(connections , 0 , sizeof(USHORT)*nVertices);
	memset(normals , 0 , sizeof(C_Normal)*nVertices);

	for(i = 0 ; i < nPolys ; i++) {
		if(indices[i].p0 > nVertices || indices[i].p1 > nVertices || indices[i].p2 > nVertices) {
			cout << nVertices << endl;
			cout << indices[i].p0 << " " << indices[i].p1 << " " << indices[i].p2 << endl;
		}

		if(invertNormals) {
			tempVec = C_Vector3::CrossProduct2(&vertices[indices[i].p0] ,
											   &vertices[indices[i].p1] ,
											   &vertices[indices[i].p2]
											  );
		} else {
			tempVec = C_Vector3::CrossProduct(&vertices[indices[i].p0] ,
											  &vertices[indices[i].p1] ,
											  &vertices[indices[i].p2]
											 );
		}
//		tempVec.Normalize ();

		connections[indices[i].p0]++;
		connections[indices[i].p1]++;
		connections[indices[i].p2]++;

		normals [ indices[i].p0 ].nx += tempVec.x;
		normals [ indices[i].p0 ].ny += tempVec.y;
		normals [ indices[i].p0 ].nz += tempVec.z;

		normals [ indices[i].p1 ].nx += tempVec.x;
		normals [ indices[i].p1 ].ny += tempVec.y;
		normals [ indices[i].p1 ].nz += tempVec.z;

		normals [ indices[i].p2 ].nx += tempVec.x;
		normals [ indices[i].p2 ].ny += tempVec.y;
		normals [ indices[i].p2 ].nz += tempVec.z;
	}

	for(i = 0 ; i < nVertices ; i++) {
		if(connections[i]) {
			normals[i].nx /= connections[i];
			normals[i].ny /= connections[i];
			normals[i].nz /= connections[i];
		}
		tempVec.x = normals[i].nx;
		tempVec.y = normals[i].ny;
		tempVec.z = normals[i].nz;
		tempVec.Normalize();
		normals[i].nx = tempVec.x;
		normals[i].ny = tempVec.y;
		normals[i].nz = tempVec.z;
	}

	delete connections;
}
