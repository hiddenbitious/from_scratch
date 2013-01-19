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

#include "3dsreader.h"
#include <assert.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

ofstream LOG_FILE_P;

bool C_3DSReader::Load3DSFile ( string filename , C_MeshGroup* group , bool invertNormals , C_Vector3 *scale )
{
	if ( !group )
		return false;

	group->reverseFaces = invertNormals;

	cout << "Reading from \"" << filename.c_str () << "\"..." << endl;

    //m_fp = fopen ( filename.c_str () , "rb" );
	fopen_s ( &m_fp , filename.c_str () , "rb" );
	if ( !m_fp )
	{
		cout << "\nFile \"" << filename.c_str () << "\" could not be opened!\n";
		return false;
	}

    struct stChunk Chunk = {0};
    ReadChunk(&Chunk);

    ParseChunk(&Chunk);

	fclose(m_fp);

	int i,cc;

	//Loop through all the meshes found in the file
	for ( i = 0 ; i < m_iNumMeshs ; i++ )
	{
		C_3DSReader::stMesh* pMesh = &(this->m_pMeshs[i]);

		//don't know why but there are some "empty" meshes... I just skip them.
		if ( !pMesh->iNumFaces || !pMesh->iNumVerts )
			continue;

		//If this mesh has multiple materials assigned to it we'll have to break it down into simpler meshes :S
		if ( pMesh->iNumMaterials > 1 )
		{
			for ( ULONG mat = 0 ; mat < pMesh->iNumMaterials ; mat++ )
			{
				C_3DSReader::stMesh pMeshTemp;

				int pp;
				ULONG index;
				pMeshTemp.iNumFaces = pMesh->facesForEachMaterial[mat].size ();
				pMeshTemp.iNumMaterials = 1;
				pMeshTemp.iNumVerts = pMesh->iNumVerts;
				if ( pMesh->pVerts && pMesh->iNumVerts ) pMeshTemp.pVerts = new stVert[pMeshTemp.iNumVerts];
				if ( pMesh->pFaces && pMeshTemp.iNumFaces ) pMeshTemp.pFaces = new stFace[pMeshTemp.iNumFaces];
				if ( pMesh->pTexs ) pMeshTemp.pTexs = new stTex[pMeshTemp.iNumVerts];

				//Copy all the geometry and the texture coordinates
				if ( pMesh->pVerts )
					memcpy ( pMeshTemp.pVerts , pMesh->pVerts , sizeof(stVert)*pMesh->iNumVerts );
				if ( pMesh->pTexs )
					memcpy ( pMeshTemp.pTexs , pMesh->pTexs , sizeof(stTex)*pMesh->iNumVerts );

				//Copy only the indices needed
				for ( pp = 0 ; pp < pMeshTemp.iNumFaces ; pp ++ )
				{
					index = pMesh->facesForEachMaterial[mat][pp];
					pMeshTemp.pFaces[pp].A = pMesh->pFaces[index].A;
					pMeshTemp.pFaces[pp].B = pMesh->pFaces[index].B;
					pMeshTemp.pFaces[pp].C = pMesh->pFaces[index].C;
					pMeshTemp.pFaces[pp].MaterialID = pMesh->pFaces[index].MaterialID;
				}

				ostringstream stm;
				string name = (string)pMesh->szMeshName;
				stm << mat;
				name += "_" + stm.str ();
				memcpy ( pMeshTemp.szMeshName , name.c_str () , name.size () );
				GetMeshInformation ( &pMeshTemp , group , invertNormals , scale );

				if ( pMeshTemp.pFaces ) delete[] pMeshTemp.pFaces;
				if ( pMeshTemp.pVerts ) delete[] pMeshTemp.pVerts;
				if ( pMeshTemp.pTexs ) delete[] pMeshTemp.pTexs;
			}
		}
		//Else things are simpler
		else// if ( pMesh->iNumMaterials == 1 )
			GetMeshInformation ( pMesh , group , invertNormals , scale );
	}

	glTexture tempTexture;
	TextureLoader texLoader;

	texLoader.SetMipMapping ( true );
	texLoader.SetTextureFilter ( txTrilinear );
	texLoader.SetTextureWrap_ST ( GL_REPEAT , GL_REPEAT );

	//Loop through the materials
	for ( int i = 0 ; i < m_iNumMaterials ; i++ )
	{
		bool textureLoaded = false;
		string textureName = (string)m_pMaterials[i].szTextureFile;

		//If the material has a texture load it
		if ( textureName.size () > 0 )
		{
			cout << "	Loading texture " << m_pMaterials[i].szTextureFile << ".......";
			textureLoaded = texLoader.LoadTextureFromDisk ( this->m_pMaterials[i].szTextureFile , &tempTexture );
			if ( textureLoaded )
				cout << "Done!" << endl;
			else
				cout << "Failed!" << endl;
		}

		//Loop through the meshes and copy the material information
		for ( cc = 0 ; cc < group->meshes.size () ; cc++ )
		{
			if ( group->meshes[cc]->material.matName.compare ( (string)m_pMaterials[i].szName ) != 0 )
				continue;

			//Copy texture filename (if any)
			group->meshes[cc]->material.texture.textureName = m_pMaterials[i].szTextureFile;

			//Copy specular information
			float r,g,b;
			string name;
			group->meshes[cc]->material.specular[0] = (m_pMaterials[i].Specular.r / 255.0f) * (m_pMaterials[i].specularLevel / 255.0f);
			group->meshes[cc]->material.specular[1] = (m_pMaterials[i].Specular.g / 255.0f) * (m_pMaterials[i].specularLevel / 255.0f);
			group->meshes[cc]->material.specular[2] = (m_pMaterials[i].Specular.b / 255.0f) * (m_pMaterials[i].specularLevel / 255.0f);
			r = group->meshes[cc]->material.specular[0];
			g = group->meshes[cc]->material.specular[1];
			b = group->meshes[cc]->material.specular[2];
			name = m_pMaterials[i].szName;


			//Copy diffuse information
			group->meshes[cc]->material.diffuse[0] = m_pMaterials[i].Color.r / 255.0f;
			group->meshes[cc]->material.diffuse[1] = m_pMaterials[i].Color.g / 255.0f;
			group->meshes[cc]->material.diffuse[2] = m_pMaterials[i].Color.b / 255.0f;

			//Copy ambient information
			group->meshes[cc]->material.ambient[0] = m_pMaterials[i].Ambient.r / 255.0f;
			group->meshes[cc]->material.ambient[1] = m_pMaterials[i].Ambient.g / 255.0f;
			group->meshes[cc]->material.ambient[2] = m_pMaterials[i].Ambient.b / 255.0f;

			//Copy glossiness and specular level
			group->meshes[cc]->material.glossiness = m_pMaterials[i].glossiness;
			group->meshes[cc]->material.specularStrength = m_pMaterials[i].specularLevel;

			//Copy texture information (if any)
			if ( !group->meshes[cc]->material.texture.textureName.compare ( textureName ) && textureName.size () && textureLoaded )
			{
				C_Material debugMat = group->meshes[cc]->material;
				group->meshes[cc]->material.texture = tempTexture;
			}
		}
	}

	cout << "Done!" << endl;
	this->Release ();

	return true;
}


bool C_3DSReader::Load3DSFile ( string filename , C_MeshGroup* group , bool invertNormals )
{
	C_Vector3 scale = C_Vector3 ( 1.0f , 1.0f , 1.0f );
	return Load3DSFile ( filename , group , invertNormals , &scale );
}


bool C_3DSReader::Load3DSFile ( string filename , C_MeshGroup* group )
{
	C_Vector3 scale = C_Vector3 ( 1.0f , 1.0f , 1.0f );
	return Load3DSFile ( filename , group , false , &scale );
}


bool C_3DSReader::Load3DSFile ( string filename , C_MeshGroup *group, C_Vector3 *scale )
{
	return Load3DSFile ( filename , group , false , scale );
}


void C_3DSReader::GetMeshInformation ( const C_3DSReader::stMesh* pMesh , C_MeshGroup* group , bool invertNormals , C_Vector3 *scale )
{
	ULONG cc;

	C_Mesh* tempMesh;
	C_TriMesh* tempTriMesh;


		tempMesh = new C_Mesh ();
		tempTriMesh = new C_TriMesh ();

		tempMesh->nVertices = pMesh->iNumVerts;
		tempMesh->nPolys = pMesh->iNumFaces;

		tempMesh->vertices = new C_Vertex [pMesh->iNumVerts];
		tempMesh->indices = new C_TriIndices [pMesh->iNumFaces];
		//tempMesh->color = new C_Color [pMesh->iNumVerts];

		// Copy vertices
		for ( cc = 0 ; cc < pMesh->iNumVerts ; cc++ )
		{
			tempMesh->vertices[cc].x = scale->x * pMesh->pVerts[cc].x;
			tempMesh->vertices[cc].y = scale->y * pMesh->pVerts[cc].y;
			tempMesh->vertices[cc].z = scale->z * pMesh->pVerts[cc].z;
		}

		// Copy indices
		for ( cc = 0 ; cc < pMesh->iNumFaces ; cc++ )
		{
			tempMesh->indices[cc].p0 = pMesh->pFaces[cc].A;
			tempMesh->indices[cc].p1 = pMesh->pFaces[cc].B;
			tempMesh->indices[cc].p2 = pMesh->pFaces[cc].C;
		}

		// Copy Texture coordinate
		if ( pMesh->pTexs != NULL )
		{
			tempMesh->texCoords = new C_TexCoord[pMesh->iNumVerts];

			for ( cc = 0 ; cc < pMesh->iNumVerts ; cc++ )
			{
				tempMesh->texCoords[cc].u = pMesh->pTexs[cc].tu;
				tempMesh->texCoords[cc].v = pMesh->pTexs[cc].tv;
			}
		}
		// Copy material name
		if ( pMesh->iNumMaterials )
			tempTriMesh->material.matName = (string)m_pMaterials[pMesh->pFaces[0].MaterialID].szName;
		// Copy meshe's name
		tempTriMesh->name = (string)pMesh->szMeshName;

		// Calculate surface normals
		tempMesh->CalcNormals ( invertNormals );
		// Make a reference to that mesh
		tempTriMesh->RefMesh ( tempMesh );
		// Calculate bounding volumes
		tempTriMesh->CalcBSphere ();
		tempTriMesh->CalcBBox ();
		// Put the mesh in the group
		group->Add ( tempTriMesh );
}


void C_3DSReader::DeleteDublicateVertices ( C_3DSReader::stVert** verts , int* nVerts )
{
	int i , j;
	C_3DSReader::stVert testVec;

	vector<C_3DSReader::stVert> vVerts;
	for ( i = 0 ; i < *nVerts ; i++ )
		vVerts.push_back ( (*verts)[i] );


	for ( i = 0 ; i < vVerts.size () - 1 ; i++ )
	{
		testVec.x = vVerts[i].x;
		testVec.y = vVerts[i].y;
		testVec.z = vVerts[i].z;

		for ( j = i+1 ; j < vVerts.size () ; j++ )
		{
			if ( vVerts[j].x == testVec.x && vVerts[j].y == testVec.y && vVerts[j].z == testVec.z )
			{
				vVerts.erase ( vVerts.begin () + j );
				j--;
			}
		}
	}

	delete[] (*verts);
	*nVerts = vVerts.size();
	(*verts) = new C_3DSReader::stVert[*nVerts];
	for ( i = 0 ; i < *nVerts ; i++ )
	{
		(*verts)[i].x = vVerts[i].x;
		(*verts)[i].y = vVerts[i].y;
		(*verts)[i].z = vVerts[i].z;
	}
}
