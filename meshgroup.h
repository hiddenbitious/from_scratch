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

#ifndef _MESHGROUP_H_
#define _MESHGROUP_H_

#include "trimesh.h"
#include <vector>
#include <GL\GLUT.h>

using namespace std;

class C_MeshGroup
{
	friend class C_3DSReader;

	public:
				
		// Number of vertices
		ULONG nVertices;
		// Number of polys (= number of indices)
		ULONG nPolys;
		// Number of triMeshes in group
		ULONG nTriMeshes;

	private:
		// The meshes the groups consists of
		// Currently using STL's vector. I'm not sure if there are any performance issues with that.
		vector<C_TriMesh*> meshes;

		// Group's position
		C_Vector3 position;
		// Group's rotation
		C_Quaternion rotationQuaternion;

		// Group's bounding volumes
		C_BSphere bsphere;
		C_BBox bbox;

		// Used in the draw method to convert the quaternion into a rotation matrix
		float rotationMatrix[16];
		float x,y,z,a;
		bool reverseFaces;

	public:
		C_MeshGroup ( void );

		// Adds a triMesh in the group
		void Add ( C_TriMesh* triMesh );

		void Translate ( const float x , const float y , const float z );
		void Translate ( const C_Vector3* vec );

		void SetPosition ( const float x , const float y , const float z );
		void SetPosition ( const C_Vector3* vec );

		C_Vector3 GetPosition ( void );

		void Rotate ( const float anglex , const float angley , const float anglez );
		void Rotate ( const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint );
		void Rotate ( const C_Quaternion* quat , const C_Vector3* rotPoint );

		void Clear ( void );

		int Draw ( C_Frustum* frustum );
		int Draw ( C_Frustum* frustum , int *nPolys );
		void Draw ( void );
		void DrawBVolumes ( void );

		void CalcBBox ( void );
		void CalcBSphere ( void );
};

#endif
