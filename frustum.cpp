#include "frustum.h"
#include <GL/glut.h>

C_Frustum::C_Frustum(void)
{
   frustumPlanes[5] = &front;
   frustumPlanes[1] = &right;
   frustumPlanes[2] = &left;
   frustumPlanes[3] = &back;
   frustumPlanes[4] = &bottom;
   frustumPlanes[0] = &top;
}

void C_Frustum::calculateFrustum(void)
{
   float mvp[16];   /// This will hold the ModelviewProjection matrix (Projection * Modelview)

   float *modl = (float *)&globalModelviewMatrix.m[0][0];
   float *proj = (float *)&globalProjectionMatrix.m[0][0];

   mvp[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   mvp[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   mvp[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   mvp[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   mvp[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   mvp[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   mvp[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   mvp[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   mvp[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   mvp[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   mvp[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   mvp[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   mvp[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   mvp[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   mvp[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   mvp[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   right.setPlane(mvp[ 3] - mvp[ 0],
                  mvp[ 7] - mvp[ 4],
                  mvp[11] - mvp[ 8],
                  mvp[15] - mvp[12]);

   left.setPlane(mvp[ 3] + mvp[ 0],
                 mvp[ 7] + mvp[ 4],
                 mvp[11] + mvp[ 8],
                 mvp[15] + mvp[12]);

   bottom.setPlane(mvp[ 3] + mvp[ 1],
                   mvp[ 7] + mvp[ 5],
                   mvp[11] + mvp[ 9],
                   mvp[15] + mvp[13]);

   top.setPlane(mvp[ 3] - mvp[ 1],
                mvp[ 7] - mvp[ 5],
                mvp[11] - mvp[ 9],
                mvp[15] - mvp[13]);

   back.setPlane(mvp[ 3] - mvp[ 2],
                 mvp[ 7] - mvp[ 6],
                 mvp[11] - mvp[10],
                 mvp[15] - mvp[14]);

   front.setPlane(mvp[ 3] + mvp[ 2],
                  mvp[ 7] + mvp[ 6],
                  mvp[11] + mvp[10],
                  mvp[15] + mvp[14]);

/// normilizing the planes is not needed unless we want to calculate exact distances from the planes
/// (aka test with bounding spheres)
//   right.normalizePlane();
//   left.normalizePlane();
//   bottom.normalizePlane();
//   top.normalizePlane();
//   back.normalizePlane();
//   front.normalizePlane();
}

bool
C_Frustum::pointInFrustum(const C_Vector3* point) const
{
   if(left.distanceFromPoint(point) <= 0) return false;
   if(right.distanceFromPoint(point) <= 0) return false;
   if(bottom.distanceFromPoint(point) <= 0) return false;
   if(top.distanceFromPoint(point) <= 0) return false;
   if(back.distanceFromPoint(point) <= 0) return false;
   if(front.distanceFromPoint(point) <= 0) return false;

   return true;
}

bool
C_Frustum::sphereInFrustum(C_Vector3* point , const float radius) const
{
   if(back.distanceFromPoint(point) <= -radius) return false;
   if(bottom.distanceFromPoint(point) <= -radius) return false;
   if(front.distanceFromPoint(point) <= -radius) return false;
   if(left.distanceFromPoint(point) <= -radius) return false;
   if(right.distanceFromPoint(point) <= -radius) return false;
   if(top.distanceFromPoint(point) <= -radius) return false;

   return true;
}


bool C_Frustum::sphereInFrustum(C_BSphere* sphere)
const
{
   C_Vector3 spherePosition = sphere->GetPosition();

   if(back.distanceFromPoint(&spherePosition) <= -sphere->GetRadius()) return false;
   if(bottom.distanceFromPoint(&spherePosition) <= -sphere->GetRadius()) return false;
   if(front.distanceFromPoint(&spherePosition) <= -sphere->GetRadius()) return false;
   if(left.distanceFromPoint(&spherePosition) <= -sphere->GetRadius()) return false;
   if(right.distanceFromPoint(&spherePosition) <= -sphere->GetRadius()) return false;
   if(top.distanceFromPoint(&spherePosition) <= -sphere->GetRadius()) return false;

   return true;
}


int C_Frustum::sphereInFrustum2(C_BSphere* sphere) const
{
   float dist;
   C_Vector3 spherePosition = sphere->GetPosition();

   for(int i = 0 ; i < 6 ; i++) {
      dist = frustumPlanes[i]->distanceFromPoint(&spherePosition);

      //Completely out of sight
      if(dist < -sphere->GetRadius()) {
         return -1;
      }

      //In sight but the sphere intersects with at least one plane
      ABS(dist);
      if(dist < sphere->GetRadius()) {
         return 0;
      }
   }

   //Fully in view.
   return 1;
}


bool C_Frustum::cubeInFrustum(const C_BBox* box) const
{
   C_Vertex vertices[8];
   box->GetVertices(vertices);

   for(int i = 0 ; i < 6 ; i++) {
      if(frustumPlanes[i]->distanceFromPoint(&vertices[0]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[1]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[2]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[3]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[4]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[5]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[6]) >= 0) continue;
      if(frustumPlanes[i]->distanceFromPoint(&vertices[7]) >= 0) continue;
      return false;
   }

   return true;
}


int C_Frustum::cubeInFrustum2(const C_BBox* box) const
{
   USHORT plane , vertex;

   C_Vector3 vertices[8];
   box->GetVertices(vertices);

   //Number of vertices found IN the frustum
   USHORT totalVerticesInFrustum = 0;

   USHORT isInside;
   USHORT verticesFRONTCurrentPlane ;

   for(plane = 0 ; plane < 6 ; plane++) {
      isInside = 1;
      verticesFRONTCurrentPlane = 8;

      for(vertex = 0 ; vertex < 8 ; vertex++) {
         //if ( !this-pointInFrustum ( &vertices[vertex] ) )
         if(frustumPlanes[plane]->distanceFromPoint(&vertices[vertex]) < 0) {
            isInside = 0;
            --verticesFRONTCurrentPlane;
         }
      }

      if(!verticesFRONTCurrentPlane) {
         return -1;
      }

      totalVerticesInFrustum += isInside;
   }

   if(totalVerticesInFrustum == 6) {
      return 1;
   }

   return 0;
}
