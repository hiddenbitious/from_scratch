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

#ifndef _TRIMESH_H_
#define _TRIMESH_H_

#include "mesh.h"
#include "bsphere.h"
#include "bbox.h"

#include <vector>
using namespace std;

typedef struct {
	float ambient[4] , diffuse[4] , specular[4];
	//Texture
	glTexture texture;
	string matName;
	unsigned char glossiness , specularStrength;
} C_Material;

class C_TriMesh {
		friend class C_3DSReader;
		friend class C_MeshGroup;

	public :

		C_Material material;
		// Texture
		glTexture texture;

		string name;

		// Object's mesh data
		C_Mesh* mesh;

		// Bounding sphere of the mesh
		C_BSphere bsphere;
		// Bounding box of the mesh
		C_BBox bbox;

		// Mesh position
		C_Vector3 position;

		// Mesh rotation
		C_Quaternion rotationQuaternion;

		// Used in the draw method to convert the quaternion into a rotation matrix
		float rotationMatrix[16];
		float x, y, z, a;

		// Draw meshes without culling
		void Draw(void);

	public:

		C_TriMesh(void);

		void Rotate();
		void Translate();

		// Reference to a mesh
		void RefMesh(C_Mesh* _mesh);

		// Clear data
		void Clear(void);

		// Translate
		void Translate(const float x , const float y , const float z);
		// Translate
		void Translate(const C_Vector3* vec);

		// Translates only bounding volumes. This is used my the group.
		// Translate as the translation of the mesh is relative but the volumes translation must be absolute.
		void TranslateBVolumes(const float x , const float y , const float z);
		void TranslateBVolumes(const C_Vector3* vec);

		void Rotate(const float x , const float y , const float z);
		void Rotate(const C_Quaternion* quat);

		void Rotate(const float x , const float y , const float z , const C_Vector3* vec);

		void RotateBVolumes(const float x , const float y , const float z , const C_Vector3* rotPoint);
		void RotateBVolumes(const C_Quaternion* quat , const C_Vector3* rotPoint);

		// Draw the mesh
		bool Draw(C_Frustum* frustum);

		void DrawBVolumes(void);

		// Calculates bounding sphere
		void CalcBSphere(void);
		// Calculates bounding box
		void CalcBBox(void);

		ULONG GetNPolys(void) { return mesh->nPolys; }
		ULONG GetNVertices(void) { return mesh->nVertices; }

};

#endif
