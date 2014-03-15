#include <stdlib.h>
#include <string.h>

#include "mesh.h"

C_BaseMesh::C_BaseMesh(void)
{
   nVertices = 0;
   nTriangles = 0;
   position.x = position.y = position.z = 0.0f;
}

C_Mesh::C_Mesh(void)
{
   vertices = NULL;
   colors = NULL;
   textCoords = NULL;
   normals = NULL;
   indices = NULL;
   next = NULL;
   texture = NULL;

   nVertices = 0;
   nTriangles = 0;
}

C_Mesh::~C_Mesh(void)
{
   if(colors)     delete[] colors;
   if(vertices)   delete[] vertices;
   if(textCoords) delete[] textCoords;
   if(normals)    delete[] normals;
   if(indices)    delete[] indices;
   if(texture)    delete texture;
}

C_MeshGroup::C_MeshGroup(void)
{
   meshes = NULL;
   nMeshes = 0;
}

C_MeshGroup::~C_MeshGroup(void)
{
   C_Mesh *oldMesh;
   C_Mesh *mesh = meshes;
   while(mesh) {
      oldMesh = mesh;
      mesh = mesh->next;
      delete oldMesh;
   }

   nMeshes = 0;
   meshes = NULL;
}

C_Mesh *
C_MeshGroup::addMesh(void)
{
   C_Mesh *newMesh = new C_Mesh();
   newMesh->next = meshes;
   meshes = newMesh;
   nMeshes++;

   return newMesh;
}

void
C_MeshGroup::draw(void)
{
//   vertex_t *mesh_transformedVerts, *tmpVerts;
	shaderManager->pushShader(shader);
   ESMatrix mat = globalModelviewMatrix;
//   esTranslate(&mat, position.x , position.y , position.z);

   shader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
   shader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

   if(shader->verticesAttribLocation >= 0)   glEnableVertexAttribArray(shader->verticesAttribLocation);
   if(shader->colorsAttribLocation >= 0)     glEnableVertexAttribArray(shader->colorsAttribLocation);
   if(shader->texCoordsAttribLocation >= 0)  glEnableVertexAttribArray(shader->texCoordsAttribLocation);

   C_Mesh *mesh = meshes;
   while(mesh) {
      /// If mesh has texture enable it
      if(mesh->texture && shader->textureUniformLocation_0 >= 0) {
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, mesh->texture->getGLtextureID());
//         glUniform1i(shader->textureUniformLocation_0, 0);
         shader->setUniform1i(UNIFORM_VARIABLE_NAME_TEXTURE_0, 0);
      } else {
         glBindTexture(GL_TEXTURE_2D, 0);
      }

      /// Manually transform mesh vertices
//      mesh_transformedVerts = (vertex_t *)malloc(mesh->nVertices * sizeof(vertex_t));
//      transformMesh(&globalMVP, mesh->vertices, mesh_transformedVerts, mesh->nVertices);
//
//      tmpVerts = mesh->vertices;
//      mesh->vertices = mesh_transformedVerts;
      mesh->draw(shader);
//      mesh->vertices = tmpVerts;
//
//      delete[] mesh_transformedVerts);

      mesh = mesh->next;
   }

   if(shader->verticesAttribLocation >= 0)   glDisableVertexAttribArray(shader->verticesAttribLocation);
   if(shader->colorsAttribLocation >= 0)     glDisableVertexAttribArray(shader->colorsAttribLocation);
   if(shader->texCoordsAttribLocation >= 0)  glDisableVertexAttribArray(shader->texCoordsAttribLocation);

   shaderManager->popShader();

   glFlush();
}

void
C_Mesh::draw(C_GLShader *shader)
{
   if(shader->verticesAttribLocation >= 0)   glVertexAttribPointer(shader->verticesAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, vertices);
   if(shader->colorsAttribLocation >= 0)     glVertexAttribPointer(shader->colorsAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, colors);
   if(shader->texCoordsAttribLocation >= 0)  glVertexAttribPointer(shader->texCoordsAttribLocation, sizeof(C_TexCoord) / sizeof(float), GL_FLOAT, GL_FALSE, 0, textCoords);

   if(!indices)
      glDrawArrays(GL_TRIANGLES, 0, nVertices);
   else
      glDrawElements(GL_TRIANGLES, 3 * nTriangles, GL_UNSIGNED_INT, indices);
}
