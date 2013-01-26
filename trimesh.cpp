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

#include "trimesh.h"

#include <stdio.h>
#include "../oglRenderer/oglDriver.h"


C_TriMesh::C_TriMesh(void)
{
	mesh = NULL;

	material.ambient[0] = material.ambient[1] = material.ambient[2] = material.ambient[3] = .2f;
	material.diffuse[0] = material.diffuse[1] = material.diffuse[2] = material.diffuse[3] = 1.0f;
	material.specular[0] = material.specular[1] = material.specular[2] = material.specular[3] = 0.0f;
}


void C_TriMesh::RefMesh(C_Mesh* _mesh)
{
	if(!_mesh) { return; }
	mesh = _mesh;
	mesh->Ref();
}


void C_TriMesh::Clear(void)
{
	if(mesh == NULL) {
		return;
	}

	mesh->UnRef();

	if(mesh->GetRef() == 0) {
		delete mesh;
	}

	mesh = NULL;
}


bool C_TriMesh::Draw(C_Frustum* frustum)
{
	if(frustum) {
		//First check is bsphere is in frustum
		if(frustum->sphereInFrustum(&bsphere)) {
			//Then check if bbox is too
			if(frustum->cubeInFrustum(&bbox)) {
				Draw();
				return true;
			}
		}
	}
	return false;
}


void C_TriMesh::Draw(void)
{
	_PushTransMatrix();


	// Translate and then rotate

	// Both codes have the same result...

	rotationQuaternion.QuaternionToAxisAngle(x , y , z , a);
	_ApplyTranslation(position.x , position.y , position.z);
	_ApplyRotation(a , x , y , z);

	/*
			rotationQuaternion.QuaternionToMatrix16 ( rotationMatrix );
			rotationMatrix[12] = position.x;
			rotationMatrix[13] = position.y;
			rotationMatrix[14] = position.z;

			_ApplyTransformation ( rotationMatrix );
	*/

	//Draw
	_DrawMesh(this);

	_PopTransMatrix();
}


void C_TriMesh::DrawBVolumes(void)
{
//	bsphere.Draw ();
	bbox.Draw();
}


void C_TriMesh::CalcBSphere(void)
{
	if(mesh == NULL) { return; }
	if(mesh->vertices == NULL) { return; }
	if(mesh->nVertices == 0) { return; }

	float minX , minY , minZ , maxX , maxY , maxZ;
	float sx , sy , sz;

	minX = maxX = mesh->vertices[mesh->indices[0].p0].x;
	minY = maxY = mesh->vertices[mesh->indices[0].p0].y;
	minZ = maxZ = mesh->vertices[mesh->indices[0].p0].z;


	sx = (maxX + minX) / 2.0f;
	sy = (maxY + minY) / 2.0f;
	sz = (maxZ + minZ) / 2.0f;

	float maxDist = C_Vector3::Magnitude(sx , sy , sz , mesh->vertices[mesh->indices[0].p0].x , mesh->vertices[mesh->indices[0].p0].y , mesh->vertices[mesh->indices[0].p0].z);
	float dist;

	for(ULONG i = 0 ; i < mesh->nPolys ; i++) {
		maxX = MAX(maxX , mesh->vertices[mesh->indices[i].p0].x);
		maxX = MAX(maxX , mesh->vertices[mesh->indices[i].p1].x);
		maxX = MAX(maxX , mesh->vertices[mesh->indices[i].p2].x);

		maxY = MAX(maxY , mesh->vertices[mesh->indices[i].p0].y);
		maxY = MAX(maxY , mesh->vertices[mesh->indices[i].p1].y);
		maxY = MAX(maxY , mesh->vertices[mesh->indices[i].p2].y);

		maxZ = MAX(maxZ , mesh->vertices[mesh->indices[i].p0].z);
		maxZ = MAX(maxZ , mesh->vertices[mesh->indices[i].p1].z);
		maxZ = MAX(maxZ , mesh->vertices[mesh->indices[i].p2].z);


		minX = MIN(minX , mesh->vertices[mesh->indices[i].p0].x);
		minX = MIN(minX , mesh->vertices[mesh->indices[i].p1].x);
		minX = MIN(minX , mesh->vertices[mesh->indices[i].p2].x);

		minY = MIN(minY , mesh->vertices[mesh->indices[i].p0].y);
		minY = MIN(minY , mesh->vertices[mesh->indices[i].p1].y);
		minY = MIN(minY , mesh->vertices[mesh->indices[i].p2].y);

		minZ = MIN(minZ , mesh->vertices[mesh->indices[i].p0].z);
		minZ = MIN(minZ , mesh->vertices[mesh->indices[i].p1].z);
		minZ = MIN(minZ , mesh->vertices[mesh->indices[i].p2].z);


		sx = (maxX + minX) / 2.0f;
		sy = (maxY + minY) / 2.0f;
		sz = (maxZ + minZ) / 2.0f;

		dist = C_Vector3::Magnitude(sx , sy , sz , maxX , maxY , maxZ);
		if(dist > maxDist) {
			maxDist = dist;
		}
	}

//	bbox.SetMax ( maxX , maxY , maxZ );
//	bbox.SetMin ( minX , minY , minZ );

	bsphere.SetPosition(sx , sy , sz);
	bsphere.SetRadius(maxDist);
}


void C_TriMesh::CalcBBox(void)
{
	if(mesh == NULL) { return; }
	if(mesh->vertices == NULL) { return; }
	if(mesh->nVertices == 0) { return; }


	float minX , minY , minZ , maxX , maxY , maxZ;

	minX = maxX = mesh->vertices[mesh->indices[0].p0].x;
	minY = maxY = mesh->vertices[mesh->indices[0].p0].y;
	minZ = maxZ = mesh->vertices[mesh->indices[0].p0].z;

	for(ULONG i = 0 ; i < mesh->nPolys ; i++) {
		maxX = MAX(maxX , mesh->vertices[mesh->indices[i].p0].x);
		maxX = MAX(maxX , mesh->vertices[mesh->indices[i].p1].x);
		maxX = MAX(maxX , mesh->vertices[mesh->indices[i].p2].x);

		maxY = MAX(maxY , mesh->vertices[mesh->indices[i].p0].y);
		maxY = MAX(maxY , mesh->vertices[mesh->indices[i].p1].y);
		maxY = MAX(maxY , mesh->vertices[mesh->indices[i].p2].y);

		maxZ = MAX(maxZ , mesh->vertices[mesh->indices[i].p0].z);
		maxZ = MAX(maxZ , mesh->vertices[mesh->indices[i].p1].z);
		maxZ = MAX(maxZ , mesh->vertices[mesh->indices[i].p2].z);


		minX = MIN(minX , mesh->vertices[mesh->indices[i].p0].x);
		minX = MIN(minX , mesh->vertices[mesh->indices[i].p1].x);
		minX = MIN(minX , mesh->vertices[mesh->indices[i].p2].x);

		minY = MIN(minY , mesh->vertices[mesh->indices[i].p0].y);
		minY = MIN(minY , mesh->vertices[mesh->indices[i].p1].y);
		minY = MIN(minY , mesh->vertices[mesh->indices[i].p2].y);

		minZ = MIN(minZ , mesh->vertices[mesh->indices[i].p0].z);
		minZ = MIN(minZ , mesh->vertices[mesh->indices[i].p1].z);
		minZ = MIN(minZ , mesh->vertices[mesh->indices[i].p2].z);
	}

	bbox.SetMax(maxX , maxY , maxZ);
	bbox.SetMin(minX , minY , minZ);
	bbox.SetVertices();
}


void C_TriMesh::Translate(const float x , const float y , const float z)
{
	position.Translate(x , y , z);
	bbox.Translate(x , y , z);
	bsphere.Translate(x , y , z);
}


void C_TriMesh::Translate(const C_Vector3 *vec)
{
	position.Translate(vec);
	bbox.Translate(vec);
	bsphere.Translate(vec);
}


void C_TriMesh::Rotate(const float anglex , const float angley , const float anglez)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	//Aplly rotation
	rotationQuaternion.Mult(&tempQuat);
	//Rotate BVOlumes
	RotateBVolumes(anglex , angley , anglez , &position);
}


void C_TriMesh::Rotate(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint)
{
	C_Quaternion tempQuat;
	tempQuat.Rotate(anglex , angley , anglez);

	//Aplly rotation
	rotationQuaternion.Mult(&tempQuat);
	//Rotate BVOlumes
	RotateBVolumes(anglex , angley , anglez , rotPoint);
}


void C_TriMesh::RotateBVolumes(const float anglex , const float angley , const float anglez , const C_Vector3* rotPoint)
{
	bbox.Rotate(anglex , angley , anglez , rotPoint);
	bsphere.Rotate(anglex , angley , anglez , rotPoint);
}


void C_TriMesh::TranslateBVolumes(const float x , const float y , const float z)
{
	bbox.Translate(x , y , z);
	bsphere.Translate(x , y , z);
}


void C_TriMesh::TranslateBVolumes(const C_Vector3 *vec)
{
	bbox.Translate(vec);
	bsphere.Translate(vec);
}
