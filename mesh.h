#ifndef _MESH_H_
#define _MESH_H_

#include "tgaLoader/texture.h"
#include "globals.h"
#include "bbox.h"

class C_Mesh {
public:
   C_Vertex       *vertices;     /// vertices
   C_Vertex       *colors;       /// colors
   C_TexCoord     *textCoords;   /// texture coordinates
   C_Normal       *normals;      /// normals
   int            *indices;      /// Vertex indices
   C_Mesh         *next;         /// Pointer to next mesh in meshGroup
   Texture        *texture;      /// Pointer to texture struct

   int            nVertices;     /// Number of vertices in mesh
   int            nFaces;        /// Number of triangles in mesh

   C_Mesh(void);
   ~C_Mesh(void);

   void draw(C_GLShader *shader);
};

class C_MeshGroup {
public:
   C_Mesh         *meshes;       /// Linked list of meshes in group
   int            nMeshes;       /// Number of meshes in group
   C_Vertex       position;      /// Group's position
   C_BBox         bbox;
   C_GLShader     *shader;

   C_MeshGroup(void);
   ~C_MeshGroup(void);

   C_Mesh *addMesh(void);        /// Creates a new mesh, adds it in the linked list and returns
                                 /// a pointer to it
   void draw(void);
};

//mesh_t *addMesh(meshGroup_t *meshGroup);
//void deleteGroup(meshGroup_t *group);
//void drawGroup(const meshGroup_t *meshGroup, int attr0, int attr1, int attr2, int uniform0);
//void drawMesh(const mesh_t *mesh, int attr0, int attr1, int attr2);

#endif
