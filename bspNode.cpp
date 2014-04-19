#include "bspNode.h"
#include "bspTree.h"
#include "bspHelperFunctions.h"
#include <GL/glut.h>

#include <stdio.h>
#include <assert.h>

#define MINIMUMRELATION			0.5f
#define MINIMUMRELATIONSCALE	2.0f

#define NPOINTS_U 5
#define NPOINTS_V 5

C_BspNode::C_BspNode(void)
{
   PRINT_FUNC_ENTRY;

   isLeaf = false;
   nodeID = 0;
   nPolys = 0;
   frontNode = NULL;
   backNode = NULL;
   fatherNode = NULL;
   geometry = NULL;
   depth = 0;
   nTriangles = 0;
   triangles = NULL;
   checkedVisibilityWith = NULL;
   visibleFrom = NULL;
}

C_BspNode::C_BspNode(poly_t** geometry , int nPolys)
{
   PRINT_FUNC_ENTRY;

   this->geometry = geometry;
   isLeaf = false;
   nodeID = 0;
   this->nPolys = nPolys;
   frontNode = NULL;
   backNode = NULL;
   fatherNode = NULL;
   nTriangles = 0;
   triangles = NULL;
   depth = 0;
   checkedVisibilityWith = NULL;
   visibleFrom = NULL;
}

C_BspNode::~C_BspNode()
{
   PRINT_FUNC_ENTRY;

   delete frontNode;
   delete backNode;

   if(checkedVisibilityWith != NULL) {
      delete[] checkedVisibilityWith;
   }

   if(visibleFrom != NULL) {
      delete[] visibleFrom;
   }

   if(triangles)
      delete[] triangles;

   /// Empty the vector
   staticObjects.clear();
}

bool
C_BspNode::IsConvex(void)
{
   poly_t** polys = geometry;

   for(int i = 0; i < nPolys; i++) {
      C_Plane plane = C_Plane(&polys[i]->pVertices[0] , &polys[i]->pVertices[1] , &polys[i]->pVertices[2]);

      for(int j = 0; j < nPolys; j++) {
         if(i == j)
            continue;
         int type = ClassifyPolygon(&plane , polys[j]);
         if(type == BACK || type == INTERSECTS) {
            return false;
         }
      }
   }
   return true;
}

bool
C_BspNode::insertStaticObject(staticTreeObject_t *staticMesh, C_Vertex *point)
{
   if(!isLeaf) {
      float side = partitionPlane.distanceFromPoint(point);

      if(FLOAT_EQ(side, 0.0f) || side >= 0.0f) {
         if(frontNode)
           return frontNode->insertStaticObject(staticMesh, point);
         else
            return false;
      } else {
         if(backNode)
            return backNode->insertStaticObject(staticMesh, point);
         else
            return false;
      }
   } else {
      for(unsigned int i = 0; i < staticObjects.size(); ++i) {
         if(staticObjects[i]->meshID == staticMesh->meshID) {
            return false;
         }
      }

      staticObjects.push_back(staticMesh);
      return true;
   }
}

void
C_BspNode::BuildBspTree(C_BspTree *tree)
{
   static int ID = 0;
   bool isConvex;
   this->tree = tree;
   C_Plane tempPlane;

   assert(tree);
   assert(depth <= tree->maxDepth);

   /// An i geometria dimiourgei kleisto horo
   /// i an ehei ftasei arketa bathia sto dendro
   /// tote einai katalili gia filo sto dentro
   if((isConvex = IsConvex()) || depth == tree->maxDepth || nPolys < 5) {
      tree->nConvexRooms += (int)isConvex;
      isLeaf = true;
      tree->nLeaves++;
      tree->leaves.push_back(this);

      if(depth > tree->depthReached) {
         tree->depthReached = depth;
      }

      if(nPolys < tree->lessPolysInNodeFound) {
         tree->lessPolysInNodeFound = nPolys;
      }


      /// Calculate leaf's bbox
      CalculateBBox();
      return;
   }


   /// Diaforetika prepei na psaksoume gia epipedo diahorismou
   SelectPartitionfromList(&tempPlane);

   partitionPlane.setPlane(&tempPlane);
   frontNode = new C_BspNode();
   frontNode->depth = depth + 1;
   frontNode->fatherNode = this;
   frontNode->partitionPlane.setPlane(&tempPlane);

   backNode = new C_BspNode();
   backNode->depth = depth + 1;
   backNode->fatherNode = this;
   backNode->partitionPlane.setPlane(&tempPlane);

   int result, nFront, nBack, i;
   nFront = nBack = 0;

   /// Classify all polygons in this node
   for(i = 0; i < nPolys; i++) {
      result = ClassifyPolygon(&partitionPlane, geometry[i]);

      if(result == FRONT) {
         ++nFront;
      } else if(result == BACK) {
         ++nBack;
      } else if(result == INTERSECTS) {
         ++nFront;
         ++nBack;
      } else if(result == COINCIDENT) {
         ++nFront;
      }
   }

   nodeID = ID++;

   /// Allocate memory
   backNode->geometry = nBack ? new poly_t *[nBack] : NULL;
   backNode->nPolys = nBack;
   backNode->nodeID = ID++;

   frontNode->geometry = nFront ? new poly_t *[nFront] : NULL;
   frontNode->nPolys = nFront;
   frontNode->nodeID = ID++;

   nFront = nBack = 0;
   tree->nNodes = ID;

   /// Distribute the geometry to the two new nodes
   for(i = 0; i < nPolys; i++) {
      result = ClassifyPolygon(&partitionPlane, geometry[i]);

      if(result == FRONT) {
         frontNode->geometry[nFront] = geometry[i];
         ++nFront;
      } else if(result == BACK) {
         backNode->geometry[nBack] = geometry[i];
         ++nBack;
      } else if(result == INTERSECTS) {
         SplitPolygon(&partitionPlane , geometry[i] , &frontNode->geometry[nFront] , &backNode->geometry[nBack]);
         tree->nSplits++;
         ++nFront;
         ++nBack;
      } else if(result == COINCIDENT) {
         frontNode->geometry[nFront] = geometry[i];
         ++nFront;
      }
   }

   /// Calculate node's bbox
   CalculateBBox();

   if(nFront) {
      frontNode->BuildBspTree(tree);

      if(frontNode->isLeaf == false) {
         delete[] frontNode->geometry;
         frontNode->geometry = NULL;
      }
   }
   if(nBack) {
      backNode->BuildBspTree(tree);

      if(backNode->isLeaf == false) {
         delete[] backNode->geometry;
         backNode->geometry = NULL;
      }
   }
}

bool
C_BspNode::SelectPartitionfromList(C_Plane* finalPlane)
{
   unsigned int nFront, nBack, nSplits, bestPlane = 0, bestSplits = INT_MAX;
   int currentPlane, i, result;
   C_Plane tempPlane;
   bool found = false;
//   poly_t **geometry = node->geometry;
//   int nPolys = node->nPolys;

   float relation, bestRelation, minRelation;
   bestRelation = 0.0f;
   minRelation = MINIMUMRELATION;

   assert(nPolys);

   while(!found) {
      for(currentPlane = 0; currentPlane < nPolys; currentPlane++) {
         if(geometry[currentPlane]->usedAsDivider == true)
            continue;

         nBack = nFront = nSplits = 0;
         tempPlane = C_Plane(&(geometry[currentPlane]->pVertices[0]),
                             &(geometry[currentPlane]->pVertices[1]),
                             &(geometry[currentPlane]->pVertices[2]));

//         printf("(%f %f %f) - (%f %f %f) - (%f %f %f)\n",
//         geometry[currentPlane]->pVertices[0].x, geometry[currentPlane]->pVertices[0].y, geometry[currentPlane]->pVertices[0].z,
//         geometry[currentPlane]->pVertices[1].x, geometry[currentPlane]->pVertices[1].y, geometry[currentPlane]->pVertices[1].z,
//         geometry[currentPlane]->pVertices[2].x, geometry[currentPlane]->pVertices[2].y, geometry[currentPlane]->pVertices[2].z);

         for(i = 0; i < nPolys; i++) {
            if(i == currentPlane)
               continue;

            result = ClassifyPolygon(&tempPlane , geometry[i]);

            if(result == FRONT) {
               nFront++;
            } else if(result == BACK) {
               nBack++;
            } else if(result == INTERSECTS) {
               nSplits++;
            }
         }

         relation = (float)MIN(nFront, nBack) / (float)MAX(nFront, nBack);
//			printf("bestSplits: %u\n", bestSplits);
//			printf("nFront: %u\n", nFront);
//			printf("nBack: %u\n", nBack);
//			printf("nSplits: %u\n", nSplits);
//			printf("relation: %f\n", relation);
//			printf("minRelation: %f\n", minRelation);
//			printf("bestRelation: %f\n", bestRelation);

         if((relation > minRelation && nSplits < bestSplits) || (nSplits == bestSplits && relation > bestRelation) ||
             (!minRelation && nSplits)) {
            finalPlane->setPlane(&tempPlane);
            bestSplits = nSplits;
            bestRelation = relation;
            bestPlane = currentPlane;
            found = true;
//            printf("\n---------------\n");
         }
      }

//		printf("******************\n");
      /// An ehoun dokimastei ola ta polygona kai den ehei brethei akoma epipedo diahorismou
      /// halarose ligo ta kritiria kai ksanapsakse
      minRelation /= MINIMUMRELATIONSCALE;
//      if(FLOAT_EQ(minRelation, 0.0f))
//         printf("Warning!\n");
   }

   geometry[bestPlane]->usedAsDivider = true;
   // Keep plane's information so we can draw it
//   debug.push_back(finalPlane->points[0]);
//   debug.push_back(finalPlane->points[1]);
//   debug.push_back(finalPlane->points[2]);

   return found;
}

void
C_BspNode::Draw(C_Camera *camera, C_BspTree* tree, bool usePVS)
{
   static unsigned long leaf = 0;

   if(!isLeaf) {
      C_Vector3 cameraPosition = camera->GetPosition();
      float side = partitionPlane.distanceFromPoint(&cameraPosition);

      if(side > 0.0f) {
         if(!usePVS) {
            if(backNode)
               backNode->Draw(camera, tree, usePVS);
         }
         if(frontNode)
            frontNode->Draw(camera, tree, usePVS);
      } else {
         if(!usePVS) {
            if(frontNode)
               frontNode->Draw(camera, tree, usePVS);
         }
         if(backNode)
            backNode->Draw(camera, tree, usePVS);
      }
   } else {
      tree->statistics.totalLeaves += PVS.size();

      if(nodeID != leaf) {
         printf("entered leaf: %lu\n", nodeID);
         leaf = nodeID;
      }

//      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      Draw(camera);
      #ifdef DRAW_BSP_GEOMETRY
      bbox.Draw(1.0f, 1.0f, 0.0f);
      DrawPointSet();
      #endif

//      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

      if(usePVS) {
         for(unsigned int i = 0 ; i < PVS.size() ; i++) {
            if(PVS[i]->drawn) {
//               assert(0);
               continue;
            }

            if(!camera->frustum->cubeInFrustum(&PVS[i]->bbox)) {
               continue;
            }

            PVS[i]->Draw(camera);
            #ifdef DRAW_BSP_GEOMETRY
            PVS[i]->bbox.Draw();
            #endif
         }
      }
   }
}

void
C_BspNode::Draw(C_Camera *camera)
{
   drawn = true;
   tree->statistics.totalStaticObjects += staticObjects.size();
   tree->statistics.leavesDrawn++;

   /// Draw bsp geometry
   #ifdef DRAW_BSP_GEOMETRY
   glEnableVertexAttribArray(bspShader->verticesAttribLocation);
   glEnableVertexAttribArray(bspShader->normalsAttribLocation);

   glVertexAttribPointer(bspShader->verticesAttribLocation, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), triangles);
   glVertexAttribPointer(bspShader->normalsAttribLocation, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), (char *)triangles + 3 * sizeof(float));
   glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);

   glDisableVertexAttribArray(bspShader->verticesAttribLocation);
   glDisableVertexAttribArray(bspShader->normalsAttribLocation);
   #endif

//   #ifdef DRAW_TREE_MESHES
   /// Draw static meshes
   for(unsigned int i = 0; i < staticObjects.size(); ++i) {
      tree->statistics.totalTriangles += staticObjects[i]->mesh.nTriangles;

      if(staticObjects[i]->drawn) {
         continue;
      }

//      if(!camera->frustum->cubeInFrustum(&staticObjects[i]->mesh.bbox)) {
//         continue;
//      }

      if(staticObjects[i]->mesh.draw(camera))
         tree->statistics.staticObjectsDrawn++;

      staticObjects[i]->drawn = true;

      tree->statistics.trianglesDrawn += staticObjects[i]->mesh.nTriangles;
   }
//   #endif
}

void
C_BspNode::CalculateBBox(void)
{
   if(geometry == NULL || !nPolys)
      return;

   float minX , minY , minZ , maxX , maxY , maxZ;

   maxX = maxY = maxZ = SMALLEST_FLOAT;
   minX = minY = minZ = GREATEST_FLOAT;

   for(int i = 0 ; i < nPolys ; i++) {
      for(int k = 0 ; k < geometry[i]->nVertices; k++) {
//         printf("%f %f %f\n", geometry[i]->pVertices[k].x, geometry[i]->pVertices[k].y, geometry[i]->pVertices[k].z);

         maxX = MAX(maxX , geometry[i]->pVertices[k].x);
         maxY = MAX(maxY , geometry[i]->pVertices[k].y);
         maxZ = MAX(maxZ , geometry[i]->pVertices[k].z);

         minX = MIN(minX , geometry[i]->pVertices[k].x);
         minY = MIN(minY , geometry[i]->pVertices[k].y);
         minZ = MIN(minZ , geometry[i]->pVertices[k].z);
      }
   }

   bbox.SetMax(maxX , maxY , maxZ);
   bbox.SetMin(minX , minY , minZ);
   bbox.SetVertices();
}

void
C_BspNode::TessellatePolygonsInLeaves(void)
{
   if(!isLeaf) {
      if(backNode)
         backNode->TessellatePolygonsInLeaves();
      if(frontNode)
         frontNode->TessellatePolygonsInLeaves();

      return;
   }

   int i;

   nTriangles = 0;
   for(i = 0 ; i < nPolys ; i++) {
      nTriangles += geometry[i]->nVertices - 2;
   }
   assert(nTriangles == 2 * nPolys);

   triangles = new triangle_vn[nTriangles];

   int currentTriangle = 0;
   for(i = 0 ; i < nPolys ; i++) {
      triangle_vn *curTri = &triangles[currentTriangle];

      switch(geometry[i]->nVertices) {
      case 3:
         assert(0);
         curTri->vertex0.x = geometry[i]->pVertices[0].x; curTri->vertex0.y = geometry[i]->pVertices[0].y; curTri->vertex0.z = geometry[i]->pVertices[0].z;
         curTri->vertex1.x = geometry[i]->pVertices[1].x; curTri->vertex1.y = geometry[i]->pVertices[1].y; curTri->vertex1.z = geometry[i]->pVertices[1].z;
         curTri->vertex2.x = geometry[i]->pVertices[2].x; curTri->vertex2.y = geometry[i]->pVertices[2].y; curTri->vertex2.z = geometry[i]->pVertices[2].z;

         curTri->normal0.x = geometry[i]->pNorms[0].x; curTri->normal0.y = geometry[i]->pNorms[0].y; curTri->normal0.z = geometry[i]->pNorms[0].z;
         curTri->normal1.x = geometry[i]->pNorms[1].x; curTri->normal1.y = geometry[i]->pNorms[1].y; curTri->normal1.z = geometry[i]->pNorms[1].z;
         curTri->normal2.x = geometry[i]->pNorms[2].x; curTri->normal2.y = geometry[i]->pNorms[2].y; curTri->normal2.z = geometry[i]->pNorms[2].z;

         currentTriangle++;
         break;

      case 4:
         /// Triangle 1
         curTri->vertex0.x = geometry[i]->pVertices[0].x; curTri->vertex0.y = geometry[i]->pVertices[0].y; curTri->vertex0.z = geometry[i]->pVertices[0].z;
         curTri->vertex1.x = geometry[i]->pVertices[1].x; curTri->vertex1.y = geometry[i]->pVertices[1].y; curTri->vertex1.z = geometry[i]->pVertices[1].z;
         curTri->vertex2.x = geometry[i]->pVertices[2].x; curTri->vertex2.y = geometry[i]->pVertices[2].y; curTri->vertex2.z = geometry[i]->pVertices[2].z;

         curTri->normal0.x = geometry[i]->pNorms[0].x; curTri->normal0.y = geometry[i]->pNorms[0].y; curTri->normal0.z = geometry[i]->pNorms[0].z;
         curTri->normal1.x = geometry[i]->pNorms[1].x; curTri->normal1.y = geometry[i]->pNorms[1].y; curTri->normal1.z = geometry[i]->pNorms[1].z;
         curTri->normal2.x = geometry[i]->pNorms[2].x; curTri->normal2.y = geometry[i]->pNorms[2].y; curTri->normal2.z = geometry[i]->pNorms[2].z;

         currentTriangle++;
         curTri = &triangles[currentTriangle];

         /// Triangle 2
         curTri->vertex0.x = geometry[i]->pVertices[2].x; curTri->vertex0.y = geometry[i]->pVertices[2].y; curTri->vertex0.z = geometry[i]->pVertices[2].z;
         curTri->vertex1.x = geometry[i]->pVertices[3].x; curTri->vertex1.y = geometry[i]->pVertices[3].y; curTri->vertex1.z = geometry[i]->pVertices[3].z;
         curTri->vertex2.x = geometry[i]->pVertices[0].x; curTri->vertex2.y = geometry[i]->pVertices[0].y; curTri->vertex2.z = geometry[i]->pVertices[0].z;

         curTri->normal0.x = geometry[i]->pNorms[2].x; curTri->normal0.y = geometry[i]->pNorms[2].y; curTri->normal0.z = geometry[i]->pNorms[2].z;
         curTri->normal1.x = geometry[i]->pNorms[3].x; curTri->normal1.y = geometry[i]->pNorms[3].y; curTri->normal1.z = geometry[i]->pNorms[3].z;
         curTri->normal2.x = geometry[i]->pNorms[0].x; curTri->normal2.y = geometry[i]->pNorms[0].y; curTri->normal2.z = geometry[i]->pNorms[0].z;

         currentTriangle++;
         break;

      default:
         assert(0);
         break;
      }
   }

   assert(nTriangles == currentTriangle);

   delete[] geometry;
   geometry = NULL;
}

void
C_BspNode::CleanUpPointSet(vector<C_Vertex>& points, bool testWithBbox, bool testWithGeometry)
{
   unsigned int cPoint = 0;

   /// Remove points outside the bbox
   if(testWithBbox) {
      while(cPoint < points.size()) {
         if(!bbox.IsInside(&points[cPoint])) {
            points.erase(points.begin() + cPoint);
            cPoint--;
         }
         cPoint++;
      }
   }

   /// Remove points coinciding with the triangles of the given node
   /// NOTE: VERY BRUTE FORCE WAY. MUST FIND SOMETHING FASTER.
   if(testWithGeometry) {
      for(int cTri = 0 ; cTri < nTriangles ; cTri++) {
         cPoint = 0;
         while(cPoint < points.size()) {
            if(PointInTriangle(&points[cPoint], &triangles[cTri])) {
               points.erase(points.begin() + cPoint);
               cPoint--;
            }

            cPoint++;
         }
      }
   }
}

//void
//C_BspNode::DistributeSamplePoints(vector<C_Vertex>& points)
//{
//   /// CLEAN UP POINTS
//   CleanUpPointSet(points, true, true);
//
//   if(isLeaf) {
//      pointSet = points;
//   } else {
//      float dist;
//      vector<C_Vertex> frontPoints, backPoints;
//
//      /// Fill in node's pointset
//      DistributePointsAlongPlane();
//      if(frontNode)
//         frontPoints = pointSet;
//      if(backNode)
//         backPoints = pointSet;
//
//      for(unsigned int i = 0 ; i < points.size() ; i++) {
//         dist = partitionPlane.distanceFromPoint(&points[i]);
//
//         if(dist > 0.0f) {
//            if(frontNode)
//               frontPoints.push_back(points[i]);
//         } else {
//            if(backNode)
//               backPoints.push_back(points[i]);
//         }
//      }
//
//      if(backNode)
//         backNode->DistributeSamplePoints(backPoints);
//      if(frontNode)
//         frontNode->DistributeSamplePoints(frontPoints);
//   }
//}


void C_BspNode::DistributePointsAlongBBox(void)
{
   C_Plane plane;
   C_Vertex bboxVertices[8];

   bbox.GetVertices(bboxVertices);

   /// Right
   plane.createFromPoints(&bboxVertices[0], &bboxVertices[4], &bboxVertices[5]);
   DistributePointsAlongPlane(&plane);

   /// Left
   plane.createFromPoints(&bboxVertices[3], &bboxVertices[7], &bboxVertices[2]);
   DistributePointsAlongPlane(&plane);

   /// Up
   plane.createFromPoints(&bboxVertices[1], &bboxVertices[5], &bboxVertices[2]);
   DistributePointsAlongPlane(&plane);

   /// Down
   plane.createFromPoints(&bboxVertices[0], &bboxVertices[4], &bboxVertices[3]);
   DistributePointsAlongPlane(&plane);
}


void
C_BspNode::DistributePointsAlongPlane(C_Plane *plane)
{
   C_Vertex min, max, tmp;
   float maxU, maxV, minU, minV;
   float tmpU, tmpV;
   maxU = maxV = SMALLEST_FLOAT;
   minU = minV = GREATEST_FLOAT;
   min.x = min.y = min.z = GREATEST_FLOAT;
   max.x = max.y = max.z = SMALLEST_FLOAT;

   /// Find where the node's partition plane intersects with the node's bounding box
   vector<C_Vertex> intersectionPoints = FindBBoxPlaneIntersections(&bbox, plane);

   for(USHORT i = 0 ; i < intersectionPoints.size() ; i++) {
      CalculateUV(plane, &intersectionPoints[i], &tmpU, &tmpV);

      if(tmpU > maxU) maxU = tmpU;
      if(tmpV > maxV) maxV = tmpV;
      if(tmpU < minU) minU = tmpU;
      if(tmpV < minV) minV = tmpV;
   }

   float nPointsU, nPointsV;
   C_Vertex bbmin, bbmax;
   bbox.GetMin(&bbmin);
   bbox.GetMax(&bbmax);
   float dist = MAX(bbmax.x - bbmin.x, bbmax.z - bbmin.z);
   nPointsU = dist > 20.0f ? (dist / 20.0f) * NPOINTS_U : NPOINTS_U;
   nPointsV = NPOINTS_V;

   assert(!FLOAT_EQ(maxU, minU));
   assert(!FLOAT_EQ(maxV, minV));
   float stepU = (maxU - minU) / nPointsU;
   float stepV = (maxV - minV) / nPointsV;

   C_Vector3 P;
   C_Vector3 A(&plane->points[0]);
   C_Vector3 B(&plane->points[1]);
   C_Vector3 C(&plane->points[2]);
   C_Vector3 v0 = C - A;
   C_Vector3 v1 = B - A;

   for(float uu = minU; uu < maxU; uu += stepU) {
      for(float vv = minV; vv < maxV; vv += stepV) {
         P = A + v0 * uu + v1 * vv;
         tmp.x = P.x;
         tmp.y = P.y;
         tmp.z = P.z;
         pointSet.push_back(tmp);
      }
   }
}

void C_BspNode::DrawPointSet(void)
{
   int n = pointSet.size();

   shaderManager->pushShader(pointShader);
      pointShader->setUniform4f("u_v4_color", 0.0f, 1.0f, 0.0f, 1.0f);

      pointShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_VIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&globalViewMatrix.m[0][0]);
      pointShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

      glEnableVertexAttribArray(pointShader->verticesAttribLocation);
      glVertexAttribPointer(pointShader->verticesAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, &pointSet[0]);
      glDrawArrays(GL_POINTS, 0, n);
      glDisableVertexAttribArray(pointShader->verticesAttribLocation);
   shaderManager->popShader();
}

bool
C_BspNode::addNodeToPVS(C_BspNode *node)
{
   for(unsigned int i = 0; i < PVS.size(); i++) {
      if(PVS[i]->nodeID == node->nodeID)
         return false;
   }

   PVS.push_back(node);
   visibleFrom[node->nodeID] = true;

   node->PVS.push_back(this);
   node->visibleFrom[nodeID] = true;

   return true;
}
