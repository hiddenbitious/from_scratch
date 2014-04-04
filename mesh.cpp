#include <stdlib.h>
#include <string.h>

#include "mesh.h"
#include "objreader/objfile.h"

C_BaseMesh::C_BaseMesh(void)
{
   PRINT_FUNC_ENTRY;

   nVertices = 0;
   nTriangles = 0;
   position.x = position.y = position.z = 0.0f;
}

C_Mesh::C_Mesh(void)
{
   PRINT_FUNC_ENTRY;

   vertices = NULL;
   colors = NULL;
   textCoords = NULL;
   normals = NULL;
   tangents = NULL;
   binormals = NULL;
   indices = NULL;
   next = NULL;
   refCounter = 1;
   texture_diffuse = NULL;
   texture_specular = NULL;
   texture_normal = NULL;
}

C_Mesh *
C_Mesh::refMesh(void)
{
   PRINT_FUNC_ENTRY;

   ++refCounter;
   return this;
}

int
C_Mesh::unRefMesh(void)
{
   PRINT_FUNC_ENTRY;

   assert(refCounter > 0);
   return --refCounter;
}

void
C_MeshGroup::softCopy(const C_MeshGroup *group)
{
   PRINT_FUNC_ENTRY;

   if(this != group) {
      /// Increase the ref counter of all the meshes
      meshes = group->meshes->refMesh();
      C_Mesh *meshList = meshes->next;
      while(meshList) {
         (void)meshList->refMesh();
         meshList = meshList->next;
      }

      nMeshes = group->nMeshes;
      nTriangles = group->nTriangles;
      nVertices = group->nVertices;
      shader = group->shader;
      bbox = group->bbox;
      position = group->position;
      matrix = group->matrix;
   }
}

C_Mesh &C_Mesh::operator= (const C_Mesh &mesh)
{
   PRINT_FUNC_ENTRY;

   if(this != &mesh) {
      if(mesh.vertices) {
         vertices = new C_Vertex[mesh.nVertices];
         memcpy(vertices, mesh.vertices, mesh.nVertices * sizeof(C_Vertex));
      }

      if(mesh.normals) {
         normals = new C_Vertex[mesh.nVertices];
         memcpy(normals, mesh.normals, mesh.nVertices* sizeof(C_Vertex));
      }

      if(mesh.tangents) {
         tangents = new C_Vertex[mesh.nVertices];
         memcpy(tangents, mesh.tangents, mesh.nVertices* sizeof(C_Vertex));
      }

      if(mesh.binormals) {
         binormals = new C_Vertex[mesh.nVertices];
         memcpy(binormals, mesh.binormals, mesh.nVertices* sizeof(C_Vertex));
      }

      if(mesh.textCoords) {
         textCoords = new C_TexCoord[mesh.nVertices];
         memcpy(textCoords, mesh.textCoords, mesh.nVertices * sizeof(C_TexCoord));
      }

      if(mesh.colors) {
         colors = new C_Color[mesh.nVertices];
         memcpy(colors, mesh.colors, mesh.nVertices * sizeof(C_Color));
      }

      assert(!indices);

      bbox = mesh.bbox;
      position = mesh.position;
      nVertices = mesh.nVertices;
      nTriangles = mesh.nTriangles;
      refCounter = mesh.refCounter;

      if(mesh.texture_diffuse) {
         texture_diffuse = mesh.texture_diffuse->refTexture();
      }
      if(mesh.texture_normal) {
         texture_normal = mesh.texture_normal->refTexture();
      }
      if(mesh.texture_specular) {
         texture_specular = mesh.texture_specular->refTexture();
      }
   }
   return *this;
}

C_Mesh::~C_Mesh(void)
{
   PRINT_FUNC_ENTRY;

   assert(!refCounter);

   if(colors)     delete[] colors;
   if(vertices)   delete[] vertices;
   if(textCoords) delete[] textCoords;
   if(normals)    delete[] normals;
   if(tangents)   delete[] normals;
   if(binormals)  delete[] normals;
   if(indices)    delete[] indices;
   if(texture_diffuse && !texture_diffuse->unrefTexture())     delete texture_diffuse;
   if(texture_normal && !texture_normal->unrefTexture())       delete texture_normal;
   if(texture_specular && !texture_specular->unrefTexture())   delete texture_specular;
}

C_MeshGroup::C_MeshGroup(void)
{
   PRINT_FUNC_ENTRY;

   meshes = NULL;
   nMeshes = 0;
   esMatrixLoadIdentity(&matrix);
}

C_MeshGroup::~C_MeshGroup(void)
{
   PRINT_FUNC_ENTRY;

   C_Mesh *oldMesh;
   C_Mesh *mesh = meshes;
   while(mesh) {
      oldMesh = mesh;
      mesh = mesh->next;
      if(!oldMesh->unRefMesh()) {
         delete oldMesh;
      }
   }

   nMeshes = 0;
   meshes = NULL;
}

C_MeshGroup &C_MeshGroup::operator= (const C_MeshGroup &group)
{
   PRINT_FUNC_ENTRY;

   if(this != &group){
      C_Mesh *mesh = group.meshes;
      C_Mesh *newMesh;

      while(mesh) {
         newMesh = addMesh();
         *newMesh = *mesh;
         mesh = mesh->next;
      }

      nMeshes = group.nMeshes;
      nTriangles = group.nTriangles;
      nVertices = group.nVertices;
      shader = group.shader;
      bbox = group.bbox;
      position = group.position;
      matrix = group.matrix;
   }
   return *this;
}

C_Mesh *
C_MeshGroup::addMesh(void)
{
   PRINT_FUNC_ENTRY;

   C_Mesh *newMesh = new C_Mesh();
   newMesh->next = meshes;
   meshes = newMesh;
   nMeshes++;

   return newMesh;
}

void
C_Mesh::applyTransformationOnVertices(const ESMatrix *mat)
{
   assert(normals);
   assert(vertices);

   for (int i = 0; i < nVertices; i++) {
      vertices[i] = math::transformPoint(mat, &vertices[i]);
      normals[i] = math::transformPoint(mat, &normals[i]);

      math::Normalize(&normals[i]);
   }
}

void
C_MeshGroup::applyTransformationOnVertices(const ESMatrix *mat)
{
   C_Mesh *mesh = meshes;
   while(mesh) {
      mesh->applyTransformationOnVertices(mat);
      mesh = mesh->next;
   }

   /// Update bboxes
   calculateBbox();
}

void
C_Mesh::calculateBbox(void)
{
   float minX = GREATEST_FLOAT, minY = GREATEST_FLOAT, minZ = GREATEST_FLOAT;
   float maxX = SMALLEST_FLOAT, maxY = SMALLEST_FLOAT, maxZ = SMALLEST_FLOAT;

   for(int i = 0; i < nVertices; ++i) {
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
C_MeshGroup::drawNormals(C_Camera *camera)
{
   glUseProgram(0);

   glPushMatrix();
   glTranslatef(position.x, position.y, position.z);

   C_Mesh *mesh = meshes;
   while(mesh) {
      mesh->drawNormals();
      mesh = mesh->next;
   }

   glPopMatrix();
}

void
C_Mesh::drawNormals(void)
{
   glBegin(GL_LINES);
   for(unsigned int i = 0; i < nTriangles; i++) {
      /// Draw normals
      glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

      glVertex3f(vertices[3 * i].x, vertices[3 * i].y, vertices[3 * i].z);
      glVertex3f(vertices[3 * i].x + normals[3 * i].x, vertices[3 * i].y + normals[3 * i].y, vertices[3 * i].z + normals[3 * i].z);

      glVertex3f(vertices[3 * i + 1].x, vertices[3 * i + 1].y, vertices[3 * i + 1].z);
      glVertex3f(vertices[3 * i + 1].x + normals[3 * i + 1].x, vertices[3 * i + 1].y + normals[3 * i + 1].y, vertices[3 * i + 1].z + normals[3 * i + 1].z);

      glVertex3f(vertices[3 * i + 2].x, vertices[3 * i + 2].y, vertices[3 * i + 2].z);
      glVertex3f(vertices[3 * i + 2].x + normals[3 * i + 2].x, vertices[3 * i + 2].y + normals[3 * i + 2].y, vertices[3 * i + 2].z + normals[3 * i + 2].z);

      /// Draw tangents
      glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

      glVertex3f(vertices[3 * i].x, vertices[3 * i].y, vertices[3 * i].z);
      glVertex3f(vertices[3 * i].x + tangents[3 * i].x, vertices[3 * i].y + tangents[3 * i].y, vertices[3 * i].z + tangents[3 * i].z);

      glVertex3f(vertices[3 * i + 1].x, vertices[3 * i + 1].y, vertices[3 * i + 1].z);
      glVertex3f(vertices[3 * i + 1].x + tangents[3 * i + 1].x, vertices[3 * i + 1].y + tangents[3 * i + 1].y, vertices[3 * i + 1].z + tangents[3 * i + 1].z);

      glVertex3f(vertices[3 * i + 2].x, vertices[3 * i + 2].y, vertices[3 * i + 2].z);
      glVertex3f(vertices[3 * i + 2].x + tangents[3 * i + 2].x, vertices[3 * i + 2].y + tangents[3 * i + 2].y, vertices[3 * i + 2].z + tangents[3 * i + 2].z);

      /// Draw binormals
      glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

      glVertex3f(vertices[3 * i].x, vertices[3 * i].y, vertices[3 * i].z);
      glVertex3f(vertices[3 * i].x + binormals[3 * i].x, vertices[3 * i].y + binormals[3 * i].y, vertices[3 * i].z + binormals[3 * i].z);

      glVertex3f(vertices[3 * i + 1].x, vertices[3 * i + 1].y, vertices[3 * i + 1].z);
      glVertex3f(vertices[3 * i + 1].x + binormals[3 * i + 1].x, vertices[3 * i + 1].y + binormals[3 * i + 1].y, vertices[3 * i + 1].z + binormals[3 * i + 1].z);

      glVertex3f(vertices[3 * i + 2].x, vertices[3 * i + 2].y, vertices[3 * i + 2].z);
      glVertex3f(vertices[3 * i + 2].x + binormals[3 * i + 2].x, vertices[3 * i + 2].y + binormals[3 * i + 2].y, vertices[3 * i + 2].z + binormals[3 * i + 2].z);
   }
   glEnd();
}

void
C_MeshGroup::draw(C_Camera *camera)
{
//   if(camera && !camera->frustum->cubeInFrustum(&bbox)) {
//      return;
//   }

	shaderManager->pushShader(shader);
	ESMatrix mat;
	esMatrixMultiply(&mat, &matrix, &globalViewMatrix);
	esTranslate(&mat, position.x, position.y, position.z);
	esMatrixMultiply(&globalMVPMatrix, &mat, &globalProjectionMatrix);

   shader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
//   shader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODEL_MATRIX, 1, GL_FALSE, (GLfloat *)&matrix.m[0][0]);
//   shader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);
   shader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MVP_MATRIX, 1, GL_FALSE, (GLfloat *)&globalMVPMatrix.m[0][0]);

   if(shader->verticesAttribLocation >= 0)   glEnableVertexAttribArray(shader->verticesAttribLocation);
   if(shader->colorsAttribLocation >= 0)     glEnableVertexAttribArray(shader->colorsAttribLocation);
   if(shader->texCoordsAttribLocation >= 0)  glEnableVertexAttribArray(shader->texCoordsAttribLocation);
   if(shader->normalsAttribLocation >= 0)    glEnableVertexAttribArray(shader->normalsAttribLocation);
   if(shader->binormalsAttribLocation >= 0)  glEnableVertexAttribArray(shader->binormalsAttribLocation);
   if(shader->tangetsAttribLocation >= 0)    glEnableVertexAttribArray(shader->tangetsAttribLocation);

   C_Mesh *mesh = meshes;
   while(mesh) {
      /// If mesh has texture enable it
      if(mesh->texture_diffuse && shader->textureUniformLocation_0 >= 0) {
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, mesh->texture_diffuse->getGLtextureID());
         shader->setUniform1i(UNIFORM_VARIABLE_NAME_TEXTURE_0, 0);

         if(mesh->texture_normal) {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, mesh->texture_normal->getGLtextureID());
            shader->setUniform1i(UNIFORM_VARIABLE_NAME_TEXTURE_1, 1);
         }
      } else {
         glBindTexture(GL_TEXTURE_2D, 0);
      }

      mesh->draw(shader);
      mesh = mesh->next;
   }

//   bbox.Draw();

   if(shader->verticesAttribLocation >= 0)   glDisableVertexAttribArray(shader->verticesAttribLocation);
   if(shader->colorsAttribLocation >= 0)     glDisableVertexAttribArray(shader->colorsAttribLocation);
   if(shader->texCoordsAttribLocation >= 0)  glDisableVertexAttribArray(shader->texCoordsAttribLocation);
   if(shader->normalsAttribLocation >= 0)    glDisableVertexAttribArray(shader->normalsAttribLocation);
   if(shader->binormalsAttribLocation >= 0)  glDisableVertexAttribArray(shader->binormalsAttribLocation);
   if(shader->tangetsAttribLocation >= 0)    glDisableVertexAttribArray(shader->tangetsAttribLocation);

   shaderManager->popShader();

   glFlush();
}

void
C_Mesh::draw(C_GLShader *shader)
{
   assert((vertices && shader->verticesAttribLocation >= 0)    || ((!vertices) && shader->verticesAttribLocation < 0));
   assert((colors && shader->colorsAttribLocation >= 0)        || ((!colors) && shader->colorsAttribLocation < 0));
   assert((textCoords && shader->texCoordsAttribLocation >= 0) || ((!textCoords) && shader->texCoordsAttribLocation< 0));

   if(vertices)   glVertexAttribPointer(shader->verticesAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, vertices);
   if(colors)     glVertexAttribPointer(shader->colorsAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, colors);
   if(textCoords) glVertexAttribPointer(shader->texCoordsAttribLocation, sizeof(C_TexCoord) / sizeof(float), GL_FLOAT, GL_FALSE, 0, textCoords);
   if(normals)    glVertexAttribPointer(shader->normalsAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, normals);
   if(binormals)  glVertexAttribPointer(shader->binormalsAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, binormals);
   if(tangents)   glVertexAttribPointer(shader->tangetsAttribLocation, sizeof(C_Vertex) / sizeof(float), GL_FLOAT, GL_FALSE, 0, tangents);

   if(!indices) {
      glDrawArrays(GL_TRIANGLES, 0, nVertices);
   } else {
      assert(0);
      glDrawElements(GL_TRIANGLES, 3 * nTriangles, GL_UNSIGNED_INT, indices);
   }
}
