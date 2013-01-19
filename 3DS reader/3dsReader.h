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

#pragma once

#include <stdio.h>
#include <vector>
#include "chunk.h"
#include <iostream>
#include "..\..\oglRenderer\textureLoader\TextureLoader.h"

using namespace std;


#include "..\meshgroup.h"


class C_3DSReader
{
    public:

        struct stMaterial
        {
            char szName[256];
			unsigned char glossiness;
			unsigned char specularLevel;
            struct
            {
                unsigned char r, g, b;
            }Color;
			struct
            {
                unsigned char r, g, b;
            }Specular;

			struct
            {
                unsigned char r, g, b;
            }Ambient;

            char szTextureFile[256];
        };
        
        struct stVert
        {
            float x, y, z;
        };    
        
        struct stFace    //3 sides of a triangle make a face
        {     
            unsigned int A, B, C;
            int MaterialID;
        };
        
        struct stTex  //texture coordinates
        {
            float tu, tv;    
        };
        
        struct stMesh
        {
            char szMeshName[256];
            int iNumVerts;
            struct stVert *pVerts;
            int iNumFaces;
            struct stFace *pFaces;
            struct stTex *pTexs;

			ULONG iNumMaterials;

			vector<vector<int>> facesForEachMaterial;
            
            //initial values
            stMesh()
            {
                iNumVerts = 0;
                pVerts = NULL;
                iNumFaces = 0;
                pFaces = NULL;
                pTexs = NULL;

				iNumMaterials = 0;
            }
        };
        
        C_3DSReader(void);
        ~C_3DSReader(void);
        
        void Release();
        
        int m_iNumMeshs;
        vector<stMesh> m_pMeshs;
        
        int m_iNumMaterials;
        vector<stMaterial> m_pMaterials;
    
    protected:
        
        struct stChunk
        {
            unsigned short id;
            unsigned int length;
            unsigned int bytesRead;
        };
        
        FILE *m_fp;
        void ParseChunk        		(struct stChunk *Chunk);
        void GetMaterialName   		(struct stChunk *Chunk);
        void GetDiffuseColour  		(struct stChunk *Chunk);
		void GetAmbientColour  		(struct stChunk *Chunk);
		void GetSpecularColour		(struct stChunk *Chunk);
		void GetMatShinines			(struct stChunk *Chunk);
		void GetMatShininesStrength (struct stChunk *Chunk);
        void GetTexFileName    		(struct stChunk *Chunk);
        void GetMeshObjectName 		(struct stChunk *Chunk);
        void ReadMeshTexCoords 		(struct stChunk *Chunk);
        void ReadMeshVertices  		(struct stChunk *Chunk);
        void ReadMeshFaces     		(struct stChunk *Chunk);
        void ReadMeshMaterials 		(struct stChunk *Chunk);
        int  GetString         		(char *pBuffer);
        void SkipChunk         		(struct stChunk *pChunk);
        void ReadChunk         		(struct stChunk *pChunk);

		void GetMeshInformation ( const C_3DSReader::stMesh *pMesh , C_MeshGroup* group , bool invertNormals , C_Vector3 *scale );
		void GetMeshInformation ( const C_3DSReader::stMesh *pMesh , C_MeshGroup* group , bool invertNormals );
		void DeleteDublicateVertices ( C_3DSReader::stVert** verts , int* nVerts );

	public:

		bool Load3DSFile ( string fileName , C_MeshGroup *group , bool invertNormals , C_Vector3 *scale );
		bool Load3DSFile ( string fileName , C_MeshGroup *group , bool invertNormals );
		bool Load3DSFile ( string fileName , C_MeshGroup *group , C_Vector3 *scale );
		bool Load3DSFile ( string fileName , C_MeshGroup *group );

};

//DEBUG
void debug_op(char *s);
void DisplayRawData(C_3DSReader* pObj);                    
