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

#include "../oglRenderer/oglDriver.h"
#include "meshgroup.h"


C_MeshGroup::C_MeshGroup ( void )
{
	nPolys = 0;
	nVertices = 0;
	nTriMeshes = 0;
}


void C_MeshGroup::Add ( C_TriMesh* triMesh )
{
	meshes.push_back ( triMesh );

	nPolys += triMesh->GetNPolys ();
	nVertices += triMesh->GetNVertices ();
	nTriMeshes++;

	CalcBBox ();
	CalcBSphere ();
}


int C_MeshGroup::Draw ( C_Frustum* frustum )
{
	int count = 0;
	bool ret;

	if ( frustum )
	{
		//First check if bsphere is in frustum
		if ( frustum->sphereInFrustum ( &bsphere ) )
		{
			//Then check if bbox is inside too
			if ( frustum->cubeInFrustum ( &bbox ) )
			{
				glPushMatrix ();

				// Translate and then rotate

					// Both codes have the same result

					rotationQuaternion.QuaternionToAxisAngle ( x , y , z , a );
					_ApplyTranslation ( position.x , position.y , position.z );
					_ApplyRotation ( a , x , y , z );

/*
					rotationQuaternion.QuaternionToMatrix16 ( rotationMatrix );
					rotationMatrix[12] = position.x;
					rotationMatrix[13] = position.y;
					rotationMatrix[14] = position.z;

					_ApplyTransformation ( rotationMatrix );
*/
					if ( reverseFaces )
						glFrontFace ( GL_CW );
					else
						glFrontFace ( GL_CCW );

					for ( int i = 0 ; i < nTriMeshes ; i++ )
					{
						ret = meshes[i]->Draw ( frustum );
						if ( ret ) count++;
					}

				glPopMatrix ();

				return count;
			}
		}
	}

	return 0;
}

int C_MeshGroup::Draw ( C_Frustum *frustum , int *nPolys )
{
	int count = 0;
	bool ret;

	if ( frustum )
	{
		//First check if bsphere is in frustum
		if ( frustum->sphereInFrustum ( &bsphere ) )
		{
			//Then check if bbox is inside too
			if ( frustum->cubeInFrustum ( &bbox ) )
			{
				glPushMatrix ();

				// Translate and then rotate

					// Both codes have the same result

					rotationQuaternion.QuaternionToAxisAngle ( x , y , z , a );
					_ApplyTranslation ( position.x , position.y , position.z );
					_ApplyRotation ( a , x , y , z );

/*
					rotationQuaternion.QuaternionToMatrix16 ( rotationMatrix );
					rotationMatrix[12] = position.x;
					rotationMatrix[13] = position.y;
					rotationMatrix[14] = position.z;

					_ApplyTransformation ( rotationMatrix );
*/
					if ( reverseFaces )
						glFrontFace ( GL_CW );
					else
						glFrontFace ( GL_CCW );

					for ( int i = 0 ; i < nTriMeshes ; i++ )
					{
						ret = meshes[i]->Draw ( frustum );
						if ( ret )
						{
							count++;
							*nPolys += meshes[i]->mesh->nPolys;
						}
					}

				glPopMatrix ();

				return count;
			}
		}
	}

	return 0;
}



void C_MeshGroup::Draw ( void )
{
	bool ret;

	glPushMatrix ();

	// Translate and then rotate

	// Both codes have the same result

	rotationQuaternion.QuaternionToAxisAngle ( x , y , z , a );
	_ApplyTranslation ( position.x , position.y , position.z );
	_ApplyRotation ( a , x , y , z );

	if ( reverseFaces )
		glFrontFace ( GL_CW );
	else
		glFrontFace ( GL_CCW );

	for ( int i = 0 ; i < nTriMeshes ; i++ )
		meshes[i]->Draw ( );

	glPopMatrix ();
}

void C_MeshGroup::DrawBVolumes ( void )
{
	glDisable ( GL_CULL_FACE );
	for ( int i = 0 ; i < nTriMeshes ; i++ )
		meshes[i]->DrawBVolumes ();

//	bbox.Draw ( 1.0f , 0.0f , 0.0f );
	bsphere.Draw ( );
	glEnable ( GL_CULL_FACE );
}


void C_MeshGroup::SetPosition ( const float x , const float y , const float z )
{
	C_Vector3 moveVec = position;
	position.SetVector ( x , y , z );

	moveVec = position - moveVec;

	for ( int i = 0 ; i < nTriMeshes ; i++ )
		meshes[i]->TranslateBVolumes ( &moveVec );

	bbox.Translate ( &moveVec );
	bsphere.Translate ( &moveVec );
}


C_Vector3 C_MeshGroup::GetPosition ( void )
{
	return position;
}


void C_MeshGroup::Translate ( const float x , const float y , const float z )
{
	position.Translate ( x , y , z );

	for ( int i = 0 ; i < nTriMeshes ; i++ )
		meshes[i]->TranslateBVolumes ( x , y , z );

	bbox.Translate ( x , y , z );
	bsphere.Translate ( x , y , z );
}


void C_MeshGroup::Translate ( const C_Vector3 *vec )
{
	position.Translate ( vec );

	for ( int i = 0 ; i < nTriMeshes ; i++ )
		meshes[i]->TranslateBVolumes ( vec );

	bbox.Translate ( vec );
	bsphere.Translate ( vec );

}

void C_MeshGroup::Rotate ( const float anglex , const float angley , const float anglez )
{
	C_Quaternion tempQuat;
	tempQuat.Rotate ( anglex , angley , anglez );

	rotationQuaternion.Mult ( &tempQuat );


	C_Vector3 newMax , newMin , tmpMax , tmpMin;

	newMax.SetVector ( SMALLEST_FLOAT , SMALLEST_FLOAT , SMALLEST_FLOAT );
	newMin.SetVector ( GREATEST_FLOAT , GREATEST_FLOAT , GREATEST_FLOAT );

	for ( int i = 0 ; i < nTriMeshes ; i++ )
	{
		//Rotate bbox of every mesh in group
		meshes[i]->RotateBVolumes ( anglex , angley , anglez , &position );

		meshes[i]->bbox.GetMax ( &tmpMax );
		meshes[i]->bbox.GetMin ( &tmpMin );

		newMax.x = MAX ( newMax.x , tmpMax.x );
		newMax.y = MAX ( newMax.y , tmpMax.y );
		newMax.z = MAX ( newMax.z , tmpMax.z );

		newMin.x = MIN ( newMin.x , tmpMin.x );
		newMin.y = MIN ( newMin.y , tmpMin.y );
		newMin.z = MIN ( newMin.z , tmpMin.z );
	}

	bbox.SetMax ( &newMax );
	bbox.SetMin ( &newMin );
	bbox.SetVertices ();

	bsphere.Rotate ( anglex , angley , anglez , &position );
}


void C_MeshGroup::Rotate ( const float anglex , const float angley , const float anglez , const C_Vector3 *rotPoint )
{
	C_Quaternion tempQuat;

	tempQuat.Rotate ( anglex , angley , anglez );
	tempQuat.RotatePoint ( &position , rotPoint );
	rotationQuaternion.Mult ( &tempQuat );

	C_Vector3 newMax , newMin , tmpMax , tmpMin;

	newMax.SetVector ( SMALLEST_FLOAT , SMALLEST_FLOAT , SMALLEST_FLOAT );
	newMin.SetVector ( GREATEST_FLOAT , GREATEST_FLOAT , GREATEST_FLOAT );

	for ( int i = 0 ; i < nTriMeshes ; i++ )
	{
		meshes[i]->RotateBVolumes ( anglex , angley , anglez , rotPoint );

		meshes[i]->bbox.GetMax ( &tmpMax );
		meshes[i]->bbox.GetMin ( &tmpMin );

		newMax.x = MAX ( newMax.x , tmpMax.x );
		newMax.y = MAX ( newMax.y , tmpMax.y );
		newMax.z = MAX ( newMax.z , tmpMax.z );

		newMin.x = MIN ( newMin.x , tmpMin.x );
		newMin.y = MIN ( newMin.y , tmpMin.y );
		newMin.z = MIN ( newMin.z , tmpMin.z );
	}

	bbox.SetMax ( &newMax );
	bbox.SetMin ( &newMin );
	bbox.SetVertices ();

	bsphere.Rotate ( anglex , angley , anglez , rotPoint );
}


void C_MeshGroup::Clear ( void )
{
	for ( int i = 0 ; i < nTriMeshes ; i++ )
	{
		meshes[i]->Clear ();
		delete meshes[i];
		meshes.pop_back ();
	}
}


void C_MeshGroup::CalcBBox ( void )
{
	C_TriMesh* currentMesh;
	float minX , minY , minZ , maxX , maxY , maxZ;

	maxX = maxY = maxZ = SMALLEST_FLOAT;
	minX = minY = minZ = GREATEST_FLOAT;

	for ( ULONG i = 0 ; i < nTriMeshes ; i++ )
	{
		currentMesh = meshes[i];

		for ( ULONG k = 0 ; k < currentMesh->mesh->nVertices ; k++ )
		{
			if ( currentMesh->mesh->vertices[k].x > maxX )
				maxX = currentMesh->mesh->vertices[k].x;
			if ( currentMesh->mesh->vertices[k].y > maxY )
				maxY = currentMesh->mesh->vertices[k].y;
			if ( currentMesh->mesh->vertices[k].z > maxZ )
				maxZ = currentMesh->mesh->vertices[k].z;

			if ( currentMesh->mesh->vertices[k].x < minX )
				minX = currentMesh->mesh->vertices[k].x;
			if ( currentMesh->mesh->vertices[k].y < minY )
				minY = currentMesh->mesh->vertices[k].y;
			if ( currentMesh->mesh->vertices[k].z < minZ )
				minZ = currentMesh->mesh->vertices[k].z;
		}
	}

	bbox.SetMax ( maxX , maxY , maxZ );
	bbox.SetMin ( minX , minY , minZ );
	bbox.SetVertices ();
}


void C_MeshGroup::CalcBSphere ( void )
{
	C_TriMesh* currentMesh;

	float minX , minY , minZ , maxX , maxY , maxZ;
	float sx , sy , sz;
	float maxDist , dist;

	maxX = maxY = maxZ = SMALLEST_FLOAT;
	minX = minY = minZ = GREATEST_FLOAT;
	maxDist = -1.0f;

	for ( ULONG i = 0 ; i < nTriMeshes ; i++ )
	{
		currentMesh = meshes[i];
		for ( ULONG k = 0 ; k < currentMesh->mesh->nVertices ; k++ )
		{
			if ( currentMesh->mesh->vertices[k].x > maxX )
				maxX = currentMesh->mesh->vertices[k].x;
			if ( currentMesh->mesh->vertices[k].y > maxY )
				maxY = currentMesh->mesh->vertices[k].y;
			if ( currentMesh->mesh->vertices[k].z > maxZ )
				maxZ = currentMesh->mesh->vertices[k].z;

			if ( currentMesh->mesh->vertices[k].x < minX )
				minX = currentMesh->mesh->vertices[k].x;
			if ( currentMesh->mesh->vertices[k].y < minY )
				minY = currentMesh->mesh->vertices[k].y;
			if ( currentMesh->mesh->vertices[k].z < minZ )
				minZ = currentMesh->mesh->vertices[k].z;

			sx = (maxX + minX) / 2.0f;
			sy = (maxY + minY) / 2.0f;
			sz = (maxZ + minZ) / 2.0f;

			dist = C_Vector3::Magnitude ( sx , sy , sz , maxX , maxY , maxZ );
			if ( dist > maxDist )
				maxDist = dist;
		}
	}

	bsphere.SetPosition ( sx , sy , sz );
	bsphere.SetRadius ( maxDist );
}
