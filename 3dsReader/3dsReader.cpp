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

#include "3dsReader.h"
#include <string.h>

C_3DSReader::C_3DSReader(void)
{
	m_iNumMeshs = 0;
	m_iNumMaterials = 0;
}

C_3DSReader::~C_3DSReader(void) {}

// ***********************
// *3ds format functions *
// ***********************

//reading and skippning chunks
void C_3DSReader::ReadChunk(struct stChunk *pChunk)
{
	unsigned short id = 0;
	unsigned int bytesRead = 0;
	unsigned int bChunkLength = 0;

	bytesRead = (unsigned int)fread(&id, 1, 2, m_fp);
	bytesRead += (unsigned int)fread(&bChunkLength, 1, 4, m_fp);

	pChunk->id = id;
	pChunk->length = bChunkLength;
	pChunk->bytesRead = bytesRead;
}

void C_3DSReader::SkipChunk(struct stChunk *pChunk)
{
	int buffer[50000] = {0};
	fread(buffer, 1, pChunk->length - pChunk->bytesRead, m_fp);
}

// reading from a buffer till NULL is found, store what we read in pBuffer

int C_3DSReader::GetString(char *pBuffer)
{
	int index = 0;
	char buffer[100] = {0};
	fread(buffer, 1, 1, m_fp);
	while(* (buffer + index++) != 0) { //read till 0 is found
		fread(buffer + index, 1, 1, m_fp);
	}
	strcpy(pBuffer, buffer);               //copy to pBuffer
	return (int)(strlen(buffer) + 1);      //return buffer size
}

// Read the material data and assign each material to its faces

void C_3DSReader::ReadMeshMaterials(struct stChunk *Chunk)
{
	//get the material name
	char str[256];
	unsigned int characterlen = GetString(str);
	Chunk->bytesRead += characterlen;

	unsigned short iNumFaces = 0;
	Chunk->bytesRead += (unsigned int)fread(&iNumFaces, 1, 2, m_fp);

	unsigned short *FaceAssignedThisMaterial = new unsigned short[iNumFaces];
	Chunk->bytesRead += (unsigned int)fread(FaceAssignedThisMaterial, 1, iNumFaces * sizeof(unsigned short), m_fp);

	//Determine which material is in our list
	int MaterialID = 0;
	for(int cc = 0; cc < m_iNumMaterials; cc++) {
		if(strcmp(str, m_pMaterials[cc].szName) == 0) {
			MaterialID = cc;
		}
	}

	struct stMesh *pMesh = &(m_pMeshs[m_iNumMeshs - 1]);
	pMesh->facesForEachMaterial.resize(pMesh->iNumMaterials + 1);
	for(int i = 0; i < iNumFaces; i++) {
		int iIndex = FaceAssignedThisMaterial[i];
		pMesh->pFaces[iIndex].MaterialID = MaterialID;
		pMesh->facesForEachMaterial[pMesh->iNumMaterials].push_back(iIndex);
	}

	pMesh->iNumMaterials++;
}

// get all the faces. take all triangle indices into the vertex array
// to put our shape together

void C_3DSReader::ReadMeshFaces(struct stChunk *Chunk)
{
	unsigned int iNumberFaces = 0;
	Chunk->bytesRead += (unsigned int)fread(&iNumberFaces, 1, 2, m_fp);

	// Each face is a triangle
	struct stXFace {
		unsigned short p1, p2, p3, visible;
	};
	struct stXFace *pFaces = new stXFace[iNumberFaces];

	Chunk->bytesRead += (unsigned int)fread(pFaces, 1, iNumberFaces * sizeof(stXFace), m_fp);

	struct stMesh *pMesh = &(m_pMeshs[m_iNumMeshs - 1]);
	pMesh->pFaces = new stFace[iNumberFaces];
	pMesh->iNumFaces = iNumberFaces;

	for(unsigned int i = 0; i < iNumberFaces; i++) {
		pMesh->pFaces[i].A = pFaces[i].p1;
		pMesh->pFaces[i].B = pFaces[i].p2;
		pMesh->pFaces[i].C = pFaces[i].p3;
	}
	delete[] pFaces;  //free up memory

	// the material info is a subchunk, parse it
	ParseChunk(Chunk);
}

//read all the vertices

void C_3DSReader::ReadMeshVertices(struct stChunk *Chunk)
{
	unsigned int iNumberVertices = 0;
	Chunk->bytesRead += (unsigned int)fread(&iNumberVertices, 1, 2, m_fp);

	// Allocate memory and dump vertices to screen
	struct stVert *pVerts = new stVert[iNumberVertices];

	Chunk->bytesRead += (unsigned int)fread((void*)pVerts, 1, iNumberVertices * sizeof(stVert), m_fp);

	struct stMesh *pMesh = &(m_pMeshs[m_iNumMeshs - 1]);
	pMesh->pVerts = pVerts;
	pMesh->iNumVerts = iNumberVertices;

	SkipChunk(Chunk);
}

//load texture coordinates

void C_3DSReader::ReadMeshTexCoords(struct stChunk *Chunk)
{
	unsigned short iNumberVertices = 0;
	Chunk->bytesRead += (unsigned int)fread(&iNumberVertices, 1, 2, m_fp);

	//Allocate memory and dump texture for each vertex on the screen
	struct stTex *pTex = new stTex[iNumberVertices];

	Chunk->bytesRead += (unsigned int)fread((void*)pTex, 1, iNumberVertices * sizeof(stTex), m_fp);
	/*	for ( int i = 0 ; i < iNumberVertices ; i++ )
		{
			Chunk->bytesRead += (unsigned int)fread((void*)&pTex->tu, 1, sizeof(float), m_fp);
			Chunk->bytesRead += (unsigned int)fread((void*)&pTex->tv, 1, sizeof(float), m_fp);
		}
	*/
	struct stMesh *pMesh = &(m_pMeshs[m_iNumMeshs - 1]);
	pMesh->pTexs = pTex;

	SkipChunk(Chunk);
}

// get object's name

void C_3DSReader::GetMeshObjectName(struct stChunk *Chunk)
{
	// these few bytes are the name
	char str[256];
	unsigned int characterlen = GetString(str);
	Chunk->bytesRead += characterlen;

	struct stMesh *pMesh = &(m_pMeshs[m_iNumMeshs - 1]);
	strcpy(pMesh->szMeshName, str);

	ParseChunk(Chunk);
}

// get texture's name

void C_3DSReader::GetTexFileName(struct stChunk *Chunk)
{
	char str[256];
	Chunk->bytesRead += GetString(str);

	struct stMaterial *pMat = &(m_pMaterials[m_iNumMaterials - 1]);
	strcpy(pMat->szTextureFile, str);
//	cout << "Found a texture: " << pMat->szTextureFile << endl;
}

// read diffuse color

void C_3DSReader::GetDiffuseColour(struct stChunk *Chunk)
{
	struct stRGB {
		unsigned char r, g, b;
	} DiffColor;

	char ChunkHeader[6];
	Chunk->bytesRead += (unsigned int)fread(ChunkHeader, 1, 6, m_fp);
	Chunk->bytesRead += (unsigned int)fread(&DiffColor, 1, 3, m_fp);
	struct stMaterial *pM = &(m_pMaterials[m_iNumMaterials - 1]);
	pM->Color.r = DiffColor.r;
	pM->Color.g = DiffColor.g;
	pM->Color.b = DiffColor.b;

	SkipChunk(Chunk);
}

void C_3DSReader::GetMatShinines(struct stChunk *Chunk)
{
	unsigned char glossiness;

	char ChunkHeader[6];
	Chunk->bytesRead += (unsigned int)fread(ChunkHeader, 1, 6, m_fp);
	Chunk->bytesRead += (unsigned int)fread(&glossiness, 1, 1, m_fp);

	struct stMaterial *pM = &(m_pMaterials[m_iNumMaterials - 1]);
	pM->glossiness = glossiness;

	SkipChunk(Chunk);
}

void C_3DSReader::GetMatShininesStrength(struct stChunk *Chunk)
{
	unsigned char specularLevel;

	char ChunkHeader[6];
	Chunk->bytesRead += (unsigned int)fread(ChunkHeader, 1, 6, m_fp);
	Chunk->bytesRead += (unsigned int)fread(&specularLevel, 1, 1, m_fp);

	struct stMaterial *pM = &(m_pMaterials[m_iNumMaterials - 1]);
	pM->specularLevel = specularLevel;

	SkipChunk(Chunk);
}

void C_3DSReader::GetSpecularColour(struct stChunk *Chunk)
{
	struct stRGB {
		unsigned char r, g, b;
	} SpecColor;

	char ChunkHeader[6];
	Chunk->bytesRead += (unsigned int)fread(ChunkHeader, 1, 6, m_fp);
	Chunk->bytesRead += (unsigned int)fread(&SpecColor, 1, 3, m_fp);
	struct stMaterial *pM = &(m_pMaterials[m_iNumMaterials - 1]);
	pM->Specular.r = SpecColor.r;
	pM->Specular.g = SpecColor.g;
	pM->Specular.b = SpecColor.b;

	SkipChunk(Chunk);
}

void C_3DSReader::GetAmbientColour(struct stChunk *Chunk)
{
	struct stRGB {
		unsigned char r, g, b;
	} AmbColor;

	char ChunkHeader[6];
	Chunk->bytesRead += (unsigned int)fread(ChunkHeader, 1, 6, m_fp);
	Chunk->bytesRead += (unsigned int)fread(&AmbColor, 1, 3, m_fp);
	struct stMaterial *pM = &(m_pMaterials[m_iNumMaterials - 1]);
	pM->Ambient.r = AmbColor.r;
	pM->Ambient.g = AmbColor.g;
	pM->Ambient.b = AmbColor.b;

	SkipChunk(Chunk);
}

//get all the materials' name

void C_3DSReader::GetMaterialName(struct stChunk *Chunk)
{
	char str[256];
	Chunk->bytesRead += GetString(str);

	struct stMaterial *pM = &(m_pMaterials[m_iNumMaterials - 1]);
	strcpy(pM->szName, str);
//	cout << "Found a material: " << pM->szName << endl;
}

// Chunk Parser, if we want to add more chunks, place their id to
// the switch statement and write a function to handle 'em or jus
// skip 'em

void C_3DSReader::ParseChunk(struct stChunk *Chunk)
{
	while(Chunk->bytesRead < Chunk->length) {
		struct stChunk tempChunk = {0};
		ReadChunk(&tempChunk);

		switch(tempChunk.id) {
				//Header our entry point
			case EDIT3DS:
				ParseChunk(&tempChunk);
				break;
				//Materials
			case MATERIAL:
				struct stMaterial newMaterial;
				m_pMaterials.push_back(newMaterial);
				m_pMaterials[m_iNumMaterials].szTextureFile[0] = '\0';
				m_iNumMaterials++;
				ParseChunk(&tempChunk);
				break;
			case MAT_NAME:
				GetMaterialName(&tempChunk);
				break;
			case MAT_DIFFUSE:
				GetDiffuseColour(&tempChunk);
				break;
			case MAT_AMBIENT:
				GetAmbientColour(&tempChunk);
				break;
			case MAT_SPECULAR:
				GetSpecularColour(&tempChunk);
				break;
			case MAT_TEXMAP:
				ParseChunk(&tempChunk);
				break;
			case MAT_TEXFLNM:
				GetTexFileName(&tempChunk);
				break;
			case MAT_SHININESS:
				GetMatShinines(&tempChunk);
				break;
			case MAT_SHININESS_STR:
				GetMatShininesStrength(&tempChunk);
				break;

				// OBJECT - MESHES
			case NAMED_OBJECT: {
					stMesh newMesh;
					m_pMeshs.push_back(newMesh);
					m_iNumMeshs++;
					GetMeshObjectName(&tempChunk);
				}
				break;
			case OBJ_MESH:
				ParseChunk(&tempChunk);
				break;
			case MESH_VERTICES:
				ReadMeshVertices(&tempChunk);
				break;
			case MESH_FACES:
				ReadMeshFaces(&tempChunk);
				break;
			case MESH_TEX_VERT:
				ReadMeshTexCoords(&tempChunk);
				break;
			case MESH_MATER:
				ReadMeshMaterials(&tempChunk);
				break;

			default:
				SkipChunk(&tempChunk);
		}
		Chunk->bytesRead += tempChunk.length;
	}
}

void C_3DSReader::Release()
{

	for(int i = 0 ; i < m_iNumMeshs ; i++) {
		if(m_pMeshs[i].pVerts) {
			delete[] m_pMeshs[i].pVerts;
			m_pMeshs[i].pVerts = NULL;
		}

		if(m_pMeshs[i].pFaces) {
			delete[] m_pMeshs[i].pFaces;
			m_pMeshs[i].pFaces = NULL;
		}

		if(m_pMeshs[i].pTexs) {
			delete[] m_pMeshs[i].pTexs;
			m_pMeshs[i].pTexs = NULL;
		}
	}

	this->m_pMeshs.clear();
	this->m_pMaterials.clear();

	this->m_pMeshs.~vector();
	this->m_pMaterials.~vector();

	this->m_iNumMeshs = 0;
	this->m_iNumMaterials = 0;
}

// debugging functions

void debug_op(char *s)
{
	FILE *fp;
	fp = fopen("debug.txt", "a+");
	fprintf(fp, "%s", s);
	fclose(fp);
}

void DisplayRawData(C_3DSReader* pObj)
{
	int cc;

	char buf[500];
	for(int i = 0; i < pObj->m_iNumMeshs; i++) {
		C_3DSReader::stMesh* pMesh = &(pObj->m_pMeshs[i]);
		sprintf(buf, "Shape: %s\n", pMesh->szMeshName);
		debug_op(buf);

		sprintf(buf, "iNumFaces: %d\n", pMesh->iNumFaces);
		debug_op(buf);
		for(cc = 0; cc < pMesh->iNumFaces; cc++) {
			sprintf(buf, "\t %d, \t %d, \t %d\n", pMesh->pFaces[cc].A, pMesh->pFaces[cc].B, pMesh->pFaces[cc].C);
			debug_op(buf);
		}

		sprintf(buf, "iNumVertices: %d\n", pMesh->iNumVerts);
		debug_op(buf);
		for(cc = 0; cc < pMesh->iNumVerts; cc++) {
			sprintf(buf, "\t %.2f, \t %.2f, \t %.2f\n", pMesh->pVerts[cc].x, pMesh->pVerts[cc].y, pMesh->pVerts[cc].z);
			debug_op(buf);
		}

		sprintf(buf, "iNumTex: %d\n", pMesh->iNumVerts);
		debug_op(buf);
		for(cc = 0; cc < pMesh->iNumVerts; cc++) {
			sprintf(buf, "\t %.2f, \t %.2f\n", pMesh->pTexs[cc].tu, pMesh->pTexs[cc].tv);
			debug_op(buf);
		}

		if(pObj->m_iNumMaterials > 0) {
			sprintf(buf, "Material vs Faces: %d\n", pMesh->iNumFaces);
			debug_op(buf);
			for(cc = 0; cc < pMesh->iNumFaces; cc++) {
				sprintf(buf, "\t MaterialID: %d,", pMesh->pFaces[cc].MaterialID);
				debug_op(buf);

				int ID = pMesh->pFaces[cc].MaterialID;
				sprintf(buf, "\t Name: %s \t Texture Filename: %s\n", pObj->m_pMaterials[ID].szName, pObj->m_pMaterials[ID].szTextureFile);
				debug_op(buf);
			}
		}
	}
}
