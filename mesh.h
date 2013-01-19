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

#ifndef _MESH_H_
#define _MESH_H_

#include "globals.h"
#include "frustum.h"
#include "..\oglRenderer\textureLoader\textureLoader.h"

class C_Mesh
{
	friend class C_3DSReader;

	public:

		//Number of vertices
		ULONG nVertices;
		//Number of polys (= number of indices)
		ULONG nPolys;

		//Vertex data
		C_Vertex* vertices;
		//Index data
		C_TriIndices* indices;
		//Color data
		C_Color* color;
		//Normal data
		C_Normal* normals;
		//Texture coords data
		C_TexCoord* texCoords;

	private:

		//Count numer of referces to this mesh
		UINT nReferences;

	public:

		C_Mesh ( void );

		ULONG GetnVertices ( void ) { return nVertices; }
		ULONG GetnPolys ( void ) { return nPolys; }

		//return number of references to this mesh
		UINT GetRef ( void ) { return nReferences; }

		//A triMesh is making a reference to this mesh. Increase counter.
		void Ref ( void );
		//Decrease references by 1.
		//If references <= 0 then the object is destroyed.
		void UnRef ( void );

		void CalcNormals ( bool invertNormals );
};

#endif
