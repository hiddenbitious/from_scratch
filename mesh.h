#ifndef _MESH_H_
#define _MESH_H_

#include "tgaLoader/texture.h"
#include "globals.h"
#include "bbox.h"
#include "camera.h"
#include "math.h"

class C_BaseMesh {
public:
   int      nVertices;
   int      nTriangles;
   C_Vertex position;
   C_BBox   bbox;

   virtual ~C_BaseMesh(void) {};
   C_BaseMesh(void);

   virtual void calculateBbox(void) = 0;
   virtual void applyTransformationOnVertices(const ESMatrix *mat) = 0;
};

class C_Mesh : public C_BaseMesh {
public:
   C_Vertex       *vertices;           /// vertices
   C_Color        *colors;             /// colors
   C_TexCoord     *textCoords;         /// texture coordinates
   C_Vertex       *normals;            /// normals
   C_Vertex       *tangents;
   C_Vertex       *binormals;
   int            *indices;            /// Vertex indices
   int            nIndices;
   C_Mesh         *next;               /// Pointer to next mesh in meshGroup

   int            refCounter;

   C_Texture      *texture_diffuse;    /// Pointer to texture struct
   C_Texture      *texture_specular;   /// Pointer to texture struct
   C_Texture      *texture_normal;     /// Pointer to texture struct

   C_Mesh(void);
   ~C_Mesh(void);

   C_Mesh &operator= (const C_Mesh &mesh);
   C_Mesh *refMesh(void);
   int unRefMesh(void);

   void draw(C_GLShader *shader);
   void calculateBbox(void);
   void applyTransformationOnVertices(const ESMatrix *mat);
};

class C_MeshGroup : public C_BaseMesh  {
public:
   C_Mesh         *meshes;       /// Linked list of meshes in group
   int            nMeshes;       /// Number of meshes in group
   C_GLShader     *shader;
   ESMatrix       matrix;

   C_MeshGroup(void);
   ~C_MeshGroup(void);

   C_MeshGroup &operator= (const C_MeshGroup &group);
   void softCopy(const C_MeshGroup *group);

   C_Mesh *addMesh(void);        /// Creates a new mesh, adds it in the linked list and returns
                                 /// a pointer to it
   void draw(C_Camera *camera);
   void calculateBbox(void);
   void applyTransformationOnVertices(const ESMatrix *mat);
   bool loadFromFile(const char *filename);
};

#endif
