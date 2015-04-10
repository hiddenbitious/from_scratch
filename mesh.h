#ifndef _MESH_H_
#define _MESH_H_

#include "tgaLoader/texture.h"
#include "globals.h"
#include "bbox.h"
#include "camera.h"
#include "math.h"

typedef enum { VERTICES_VBO, NORMALS_VBO, BINORMALS_VBO, TANGENTS_VBO, COLORS_VBO, TEXCOORDS_VBO, MAX_VBOS_PER_OBJECT } mesh_vbos;

class C_BaseMesh {
public:
   int            nVertices;
   int            nTriangles;
   C_BBox         bbox;

   virtual ~C_BaseMesh(void) {};
   C_BaseMesh(void);

   virtual void calculateBbox(void) = 0;
   virtual void applyTransformationOnVertices(const ESMatrix *mat) = 0;

   virtual void translate(float x, float y, float z) = 0;
   virtual void translate(C_Vertex *translation) = 0;

   virtual void rotate(float x, float y, float z) = 0;
   virtual void rotate(C_Vertex *rotation) = 0;
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
   C_MeshGroup    *group;

   int            refCounter;

   C_Texture      *texture_diffuse;    /// Pointer to texture struct
   C_Texture      *texture_specular;   /// Pointer to texture struct
   C_Texture      *texture_normal;     /// Pointer to texture struct

   unsigned       vbos[MAX_VBOS_PER_OBJECT];

   C_Mesh(void);
   ~C_Mesh(void);

   C_Mesh &operator= (const C_Mesh &mesh);
   C_Mesh *refMesh(void);
   int unRefMesh(void);

   virtual void translate(float x, float y, float z);
   virtual void translate(C_Vertex *translation);
   virtual void rotate(float x, float y, float z);
   virtual void rotate(C_Vertex *rotation);

   void draw(C_GLShader *shader);
   void drawNormals(void);
   void calculateBbox(void);
   void applyTransformationOnVertices(const ESMatrix *mat);

};

class C_MeshGroup : public C_BaseMesh  {
public:
   C_Mesh         *meshes;                /// Linked list of meshes in group
   int            nMeshes;                /// Number of meshes in group
   C_GLShader     *shader;
   C_Vertex       position;
   ESMatrix       matrix;
   bool           applyFrustumCulling;    /// Don't apply frustum culling on low poly meshes

   C_MeshGroup(void);
   ~C_MeshGroup(void);

   C_MeshGroup &operator= (const C_MeshGroup &group);
   void softCopy(const C_MeshGroup *group);

   C_Mesh *addMesh(void);        /// Creates a new mesh, adds it in the linked list and returns
                                 /// a pointer to it
   bool draw(C_Camera *camera);
   void drawNormals(C_Camera *camera);
   void calculateBbox(void);
   void applyTransformationOnVertices(const ESMatrix *mat);
   bool loadFromFile(const char *filename);

   bool initVBOS();

   virtual void translate(float x, float y, float z);
   virtual void translate(C_Vertex *translation);

   virtual void rotate(float x, float y, float z);
   virtual void rotate(C_Vertex *rotation);

private:
   C_Quaternion   rotationQuat;
   bool           rotated;
};

#endif
