#ifndef _MESH_H_
#define _MESH_H_

#include "tgaLoader/texture.h"
#include "globals.h"
#include "bbox.h"
#include "math.h"

class C_BaseMesh {
public:
   int nVertices;
   int nTriangles;
   C_Vertex position;
   C_BBox bbox;

   virtual ~C_BaseMesh(void) {};
   C_BaseMesh(void);

   virtual void calculateBbox(void) = 0;
   virtual void applyTransformationOnVertices(const ESMatrix *mat) = 0;
//   void drawbbox(void) { bbox.Draw(); };
};

class C_Mesh : public C_BaseMesh {
public:
   C_Vertex       *vertices;     /// vertices
   C_Vertex       *colors;       /// colors
   C_TexCoord     *textCoords;   /// texture coordinates
   C_Normal       *normals;      /// normals
   int            *indices;      /// Vertex indices
   C_Mesh         *next;         /// Pointer to next mesh in meshGroup
   C_Texture      *texture;      /// Pointer to texture struct

   C_Mesh(void);
   ~C_Mesh(void);

   void draw(C_GLShader *shader);
   void calculateBbox(void);
   void applyTransformationOnVertices(const ESMatrix *mat);
};

class C_MeshGroup : public C_BaseMesh  {
public:
   C_Mesh         *meshes;       /// Linked list of meshes in group
   int            nMeshes;       /// Number of meshes in group
   int            nTriangles;    /// Total number of triangles in all meshes
   int            nVertices;     /// Total number of vertices in all meshes
   C_GLShader     *shader;

   C_MeshGroup(void);
   ~C_MeshGroup(void);

   C_Mesh *addMesh(void);        /// Creates a new mesh, adds it in the linked list and returns
                                 /// a pointer to it
   void draw(void);
   void calculateBbox(void);
   void applyTransformationOnVertices(const ESMatrix *mat);
   bool loadFromFile(const char *filename);
};

#endif
