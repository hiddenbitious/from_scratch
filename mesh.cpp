#include <stdlib.h>
#include <string.h>

#include "mesh.h"
#include "objreader/objfile.h"

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
C_Mesh::applyTransformationOnVertices(const ESMatrix *mat)
{
   for (int i = 0; i < nVertices; i++)
      vertices[i] = math::transformPoint(mat, &vertices[i]);
}

void
C_MeshGroup::applyTransformationOnVertices(const ESMatrix *mat)
{
   C_Mesh *mesh = meshes;
   while(mesh) {
      mesh->applyTransformationOnVertices(mat);
      mesh = mesh->next;
   }
}

void
C_Mesh::calculateBbox(void)
{
   float minX = GREATEST_FLOAT, minY = GREATEST_FLOAT, minZ = GREATEST_FLOAT;
   float maxX = SMALLEST_FLOAT, maxY = SMALLEST_FLOAT, maxZ = SMALLEST_FLOAT;

   for(unsigned int i = 0; i < nVertices; ++i) {
      if(vertices[i].x > maxX) maxX = vertices[i].x;
      if(vertices[i].y > maxY) maxY = vertices[i].y;
      if(vertices[i].z > maxZ) maxZ = vertices[i].z;

      if(vertices[i].x < minX) minX = vertices[i].x;
      if(vertices[i].y < minY) minY = vertices[i].y;
      if(vertices[i].z < minZ) minZ = vertices[i].z;
   }

   bbox.SetMax(maxX , maxY , maxZ);
   bbox.SetMin(minX , minY , minZ);
   bbox.SetVertices();
}

void
C_MeshGroup::calculateBbox(void)
{
   float minX = GREATEST_FLOAT, minY = GREATEST_FLOAT, minZ = GREATEST_FLOAT;
   float maxX = SMALLEST_FLOAT, maxY = SMALLEST_FLOAT, maxZ = SMALLEST_FLOAT;
   C_Vertex min, max;

   C_Mesh *mesh = meshes;
   while(mesh) {
      mesh->calculateBbox();

      mesh->bbox.GetMax(&max);
      mesh->bbox.GetMin(&min);

      if(min.x < minX) minX = min.x;
      if(min.y < minY) minY = min.y;
      if(min.z < minZ) minZ = min.z;

      if(max.x > maxX) maxX = max.x;
      if(max.y > maxY) maxY = max.y;
      if(max.z > maxZ) maxZ = max.z;

      mesh = mesh->next;
   }

   bbox.SetMax(maxX , maxY , maxZ);
   bbox.SetMin(minX , minY , minZ);
   bbox.SetVertices();
}

bool
C_MeshGroup::loadFromFile(const char *filename)
{
   glmReadOBJ(filename, this);
   calculateBbox();

   return true;
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

   bbox.Draw();

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
