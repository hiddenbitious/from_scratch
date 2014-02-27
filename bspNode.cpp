#include "bspNode.h"
#include "bspTree.h"
#include "bspHelperFunctions.h"
#include <GL/glut.h>

#include <stdio.h>
#include <assert.h>

#define MINIMUMRELATION			0.5f
#define MINIMUMRELATIONSCALE	2.0f

#define NPOINTS_U 20
#define NPOINTS_V 20

C_BspNode::C_BspNode(void)
{
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
   bbox = NULL;
}

C_BspNode::C_BspNode(poly_t** geometry , int nPolys)
{
   this->geometry = geometry;
   isLeaf = false;
   nodeID = 0;
   this->nPolys = nPolys;
   frontNode = NULL;
   backNode = NULL;
   fatherNode = NULL;
   bbox = NULL;
   nTriangles = 0;
   triangles = NULL;
   depth = 0;
   checkedVisibilityWith = NULL;
   visibleFrom = NULL;
}

C_BspNode::~C_BspNode()
{
   delete frontNode;
   delete backNode;

   if(checkedVisibilityWith != NULL) {
      delete[] checkedVisibilityWith;
   }
   if(visibleFrom != NULL) {
      delete[] visibleFrom;
   }

   delete bbox;
}

int
C_BspNode::ClassifyVertex(C_Plane *plane , C_Vertex *vertex)
{
   float dist = plane->distanceFromPoint(vertex);

   if(FLOAT_EQ(dist, 0.0f)) {
      return COINCIDENT;
   } else if(dist > EPSILON) {
      return FRONT;
   } else {
      return BACK;
   }
}

int
C_BspNode::ClassifyPolygon(C_Plane *plane, poly_t *polygon)
{
   int front = 0, back = 0;

   for(int i = 0; i < polygon->nVertices; i++) {
      int whereIs = ClassifyVertex(plane , &(polygon->pVertices[i]));

      if(whereIs == FRONT) {
         front++;
      } else if(whereIs == BACK) {
         back++;
      }
   }

   if(front && !back) {
      return FRONT;
   } else if(!front && back) {
      return BACK;
   } else if(!front && !back) {
      return COINCIDENT;
   } else {
      return INTERSECTS;
   }
}

bool
C_BspNode::IsConvex(C_BspNode *node)
{
   poly_t** polys = node->geometry;
   int nPolys = node->nPolys;

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

void
C_BspNode::BuildBspTree(C_BspNode* node, C_BspTree *tree)
{
   static int ID = 0;
   bool isConvex;
   node->tree = tree;
   C_Plane tempPlane;

   assert(node);
   assert(tree);
   assert(node->depth <= tree->maxDepth);

   /// An i geometria dimiourgei kleisto horo
   /// i an ehei ftasei arketa bathia sto dendro
   /// tote einai katalili gia filo sto dentro
   if((isConvex = C_BspNode::IsConvex(node)) || node->depth == tree->maxDepth) {
      tree->nConvexRooms += (int)isConvex;
      node->isLeaf = true;
      tree->nLeaves++;
      tree->leaves.push_back(node);

      if(node->depth > tree->depthReached) {
         tree->depthReached = node->depth;
      }

      if(node->nPolys < tree->lessPolysInNodeFound) {
         tree->lessPolysInNodeFound = node->nPolys;
      }

      /// Calculate leaf's bbox
      node->CalculateBBox();
      return;
   }

   /// Diaforetika prepei na psaksoume gia epipedo diahorismou
   SelectPartitionfromList(node, &tempPlane);

   node->partitionPlane.setPlane(&tempPlane);
   node->frontNode = new C_BspNode();
   node->frontNode->depth = node->depth + 1;
   node->frontNode->fatherNode = node;

   node->backNode = new C_BspNode();
   node->backNode->depth = node->depth + 1;
   node->backNode->fatherNode = node;

   int result, nFront, nBack, i;
   nFront = nBack = 0;

   /// Classify all polygons in this node
   for(i = 0; i < node->nPolys; i++) {
      result = C_BspNode::ClassifyPolygon(&node->partitionPlane , node->geometry[i]);

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

   node->nodeID = ID++;

   /// Allocate memory
   node->backNode->geometry = nBack ? new poly_t *[nBack] : NULL;
   node->backNode->nPolys = nBack;
   node->backNode->nodeID = ID++;

   node->frontNode->geometry = nFront ? new poly_t *[nFront] : NULL;
   node->frontNode->nPolys = nFront;
   node->frontNode->nodeID = ID++;

   nFront = nBack = 0;
   tree->nNodes = ID;

   /// Distribute the geometry to the two new nodes
   for(i = 0; i < node->nPolys; i++) {
      result = C_BspNode::ClassifyPolygon(&node->partitionPlane, node->geometry[i]);

      if(result == FRONT) {
         node->frontNode->geometry[nFront] = node->geometry[i];
         ++nFront;
      } else if(result == BACK) {
         node->backNode->geometry[nBack] = node->geometry[i];
         ++nBack;
      } else if(result == INTERSECTS) {
         C_BspNode::SplitPolygon(&node->partitionPlane , node->geometry[i] , &node->frontNode->geometry[nFront] , &node->backNode->geometry[nBack]);
         tree->nSplits++;
         ++nFront;
         ++nBack;
      } else if(result == COINCIDENT) {
         node->frontNode->geometry[nFront] = node->geometry[i];
         ++nFront;
      }
   }

   /// Calculate node's bbox
   node->CalculateBBox();

   if(nFront) {
      C_BspNode::BuildBspTree(node->frontNode, tree);

      if(node->frontNode->isLeaf == false) {
         delete[] node->frontNode->geometry;
         node->frontNode->geometry = NULL;
      }
   }
   if(nBack) {
      C_BspNode::BuildBspTree(node->backNode, tree);

      if(node->backNode->isLeaf == false) {
         delete[] node->backNode->geometry;
         node->backNode->geometry = NULL;
      }
   }
}

bool
C_BspNode::SelectPartitionfromList(C_BspNode *node, C_Plane* finalPlane)
{
   unsigned int nFront, nBack, nSplits, bestPlane = 0, bestSplits = INT_MAX;
   C_Plane tempPlane;
   bool found = false;
   poly_t **geometry = node->geometry;
   int nPolys = node->nPolys;

   float relation, bestRelation, minRelation;
   bestRelation = 0.0f;
   minRelation = MINIMUMRELATION;

   assert(nPolys);

   while(!found) {
      for(int currentPlane = 0; currentPlane < nPolys; currentPlane++) {
         if(geometry[currentPlane]->usedAsDivider == true)
            continue;

         nBack = nFront = nSplits = 0;
         tempPlane = C_Plane(&(geometry[currentPlane]->pVertices[0]),
                             &(geometry[currentPlane]->pVertices[1]),
                             &(geometry[currentPlane]->pVertices[2]));

         for(int i = 0; i < nPolys; i++) {
            if(i == currentPlane)
               continue;

            int result = C_BspNode::ClassifyPolygon(&tempPlane , geometry[i]);

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

         if((relation > minRelation && nSplits < bestSplits) || (nSplits == bestSplits && relation > bestRelation)) {
            finalPlane->setPlane(&tempPlane);
            bestSplits = nSplits;
            bestRelation = relation;
            bestPlane = currentPlane;
            found = true;
//				printf("****\n");
         }
      }

      /// An ehoun dokimastei ola ta polygona kai den ehei brethei akoma epipedo diahorismou
      /// halarose ligo ta kritiria kai ksanapsakse
      minRelation /= MINIMUMRELATIONSCALE;
   }

   geometry[bestPlane]->usedAsDivider = true;
   // Keep plane's information so we can draw it
//   debug.push_back(finalPlane->points[0]);
//   debug.push_back(finalPlane->points[1]);
//   debug.push_back(finalPlane->points[2]);

   return found;
}

void
C_BspNode::SplitPolygon(C_Plane *plane , poly_t *polygon , poly_t **front , poly_t **back)
{
   vector<C_Vertex> newFront;
   vector<C_Vertex> newBack;
   C_Vertex intersectionPoint;

   C_Vertex ptA , ptB;
   float sideA , sideB;

   int i;
   int count = polygon->nVertices;

   ptA = polygon->pVertices[count - 1];
   sideA = plane->distanceFromPoint(&ptA);

   for(i = 0; i < count; i++) {
      ptB = polygon->pVertices[i];
      sideB = plane->distanceFromPoint(&ptB);

      if(sideB > 0.0f) {
         if(sideA < 0.0f) {
            intersectionPoint = FindIntersectionPoint(&ptA , &ptB , plane);

            newFront.push_back(intersectionPoint);
            newBack.push_back(intersectionPoint);
         }
         newFront.push_back(ptB);
      } else if(sideB < 0.0f) {
         if(sideA > 0.0f) {
            intersectionPoint = FindIntersectionPoint(&ptA , &ptB , plane);

            newFront.push_back(intersectionPoint);
            newBack.push_back(intersectionPoint);
         }
         newBack.push_back(ptB);
      } else {
         newFront.push_back(ptB);
         newBack.push_back(ptB);
      }

      ptA = ptB;
      sideA = sideB;
   }

   *front = new poly_t();
   (*front)->nVertices = newFront.size();
   (*front)->pNorms = new C_Vertex[(*front)->nVertices];
   (*front)->pVertices = new C_Vertex[(*front)->nVertices];
   (*front)->usedAsDivider = polygon->usedAsDivider;

   for(i = 0 ; i < (*front)->nVertices ; i++) {
      (*front)->pVertices[i].x = newFront[i].x;
      (*front)->pVertices[i].y = newFront[i].y;
      (*front)->pVertices[i].z = newFront[i].z;

      (*front)->pNorms[i].x = polygon->pNorms[0].x;
      (*front)->pNorms[i].y = polygon->pNorms[0].y;
      (*front)->pNorms[i].z = polygon->pNorms[0].z;
   }

   *back = new poly_t();
   (*back)->nVertices = newBack.size();
   (*back)->pNorms = new C_Vertex[(*back)->nVertices];
   (*back)->pVertices = new C_Vertex[(*back)->nVertices];
   (*back)->usedAsDivider = polygon->usedAsDivider;

   for(i = 0 ; i < (*back)->nVertices ; i++) {
      (*back)->pVertices[i].x = newBack[i].x;
      (*back)->pVertices[i].y = newBack[i].y;
      (*back)->pVertices[i].z = newBack[i].z;

      (*back)->pNorms[i].x = polygon->pNorms[0].x;
      (*back)->pNorms[i].y = polygon->pNorms[0].y;
      (*back)->pNorms[i].z = polygon->pNorms[0].z;
   }

   return;
}

void
C_BspNode::Draw(C_Vector3* cameraPosition , C_BspNode* node , C_BspTree* tree, bool usePVS)
{
   static int lastNode = -1;
   if(!node->isLeaf) {
      float side = node->partitionPlane.distanceFromPoint(cameraPosition);

      if(side > 0.0f) {
         if(!usePVS)
            C_BspNode::Draw(cameraPosition, node->backNode, tree, usePVS);
         C_BspNode::Draw(cameraPosition, node->frontNode, tree, usePVS);
      } else {
         if(!usePVS)
            C_BspNode::Draw(cameraPosition, node->frontNode, tree, usePVS);
         C_BspNode::Draw(cameraPosition, node->backNode, tree, usePVS);
      }
   } else {
      if(node->drawn) {
         return;
      }

      if(lastNode != node->nodeID) {
         lastNode = node->nodeID;
         printf("Entered leaf: %d\n", lastNode);
      }

      node->drawn = true;
      node->Draw(bspShader);
      node->DrawPointSet();
      polyCount += node->nPolys;

      if(usePVS) {
         for(unsigned int i = 0 ; i < node->PVS.size() ; i++) {
            if(node->PVS[i]->drawn) {
               continue;
            }

            node->PVS[i]->drawn = true;
            node->PVS[i]->Draw(bspShader);

            node->PVS[i]->bbox->Draw();
   //			node->PVS[i]->DrawPointSet();

            polyCount += node->PVS[i]->nPolys;
         }
      }
   }
}

void
C_BspNode::Draw(C_GLShader *shader)
{
   /// Vertices
	glVertexAttribPointer(shader->verticesAttribLocation, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), triangles);
	/// Normals
	glVertexAttribPointer(shader->normalsAttribLocation, 3, GL_FLOAT, GL_FALSE, (3 + 3) * sizeof(float), (char *)triangles + 3 * sizeof(float));

   glDrawArrays(GL_TRIANGLES, 0, nTriangles * 3);
}

void
C_BspNode::CalculateBBox(void)
{
   if(geometry == NULL || !nPolys)
      return;

   assert(!bbox);
   if(bbox) delete bbox;
   bbox = new C_BBox();

   float minX , minY , minZ , maxX , maxY , maxZ;

   maxX = maxY = maxZ = SMALLEST_FLOAT;
   minX = minY = minZ = GREATEST_FLOAT;

   for(int i = 0 ; i < nPolys ; i++) {
      for(int k = 0 ; k < geometry[i]->nVertices; k++) {
         maxX = MAX(maxX , geometry[i]->pVertices[k].x);
         maxY = MAX(maxY , geometry[i]->pVertices[k].y);
         maxZ = MAX(maxZ , geometry[i]->pVertices[k].z);

         minX = MIN(minX , geometry[i]->pVertices[k].x);
         minY = MIN(minY , geometry[i]->pVertices[k].y);
         minZ = MIN(minZ , geometry[i]->pVertices[k].z);
      }
   }

   bbox->SetMax(maxX , maxY , maxZ);
   bbox->SetMin(minX , minY , minZ);
   bbox->SetVertices();
}

void
C_BspNode::TessellatePolygonsInLeaves(C_BspNode* node)
{
   if(!node->isLeaf) {
      TessellatePolygonsInLeaves(node->backNode);
      TessellatePolygonsInLeaves(node->frontNode);

      return;
   }

   int i;

   node->nTriangles = 0;
   for(i = 0 ; i < node->nPolys ; i++) {
      node->nTriangles += node->geometry[i]->nVertices - 2;
   }
   assert(node->nTriangles == 2 * node->nPolys);

   node->triangles = new triangle_vn[node->nTriangles];

   int currentTriangle = 0;
   for(i = 0 ; i < node->nPolys ; i++) {
      triangle_vn *curTri = &node->triangles[currentTriangle];

      switch(node->geometry[i]->nVertices) {
      case 3:
         assert(0);
         curTri->vertex0.x = node->geometry[i]->pVertices[0].x; curTri->vertex0.y = node->geometry[i]->pVertices[0].y; curTri->vertex0.z = node->geometry[i]->pVertices[0].z;
         curTri->vertex1.x = node->geometry[i]->pVertices[1].x; curTri->vertex1.y = node->geometry[i]->pVertices[1].y; curTri->vertex1.z = node->geometry[i]->pVertices[1].z;
         curTri->vertex2.x = node->geometry[i]->pVertices[2].x; curTri->vertex2.y = node->geometry[i]->pVertices[2].y; curTri->vertex2.z = node->geometry[i]->pVertices[2].z;

         curTri->normal0.x = node->geometry[i]->pNorms[0].x; curTri->normal0.y = node->geometry[i]->pNorms[0].y; curTri->normal0.z = node->geometry[i]->pNorms[0].z;
         curTri->normal1.x = node->geometry[i]->pNorms[1].x; curTri->normal1.y = node->geometry[i]->pNorms[1].y; curTri->normal1.z = node->geometry[i]->pNorms[1].z;
         curTri->normal2.x = node->geometry[i]->pNorms[2].x; curTri->normal2.y = node->geometry[i]->pNorms[2].y; curTri->normal2.z = node->geometry[i]->pNorms[2].z;

         currentTriangle++;
         break;

      case 4:
         /// Triangle 1
         curTri->vertex0.x = node->geometry[i]->pVertices[0].x; curTri->vertex0.y = node->geometry[i]->pVertices[0].y; curTri->vertex0.z = node->geometry[i]->pVertices[0].z;
         curTri->vertex1.x = node->geometry[i]->pVertices[1].x; curTri->vertex1.y = node->geometry[i]->pVertices[1].y; curTri->vertex1.z = node->geometry[i]->pVertices[1].z;
         curTri->vertex2.x = node->geometry[i]->pVertices[2].x; curTri->vertex2.y = node->geometry[i]->pVertices[2].y; curTri->vertex2.z = node->geometry[i]->pVertices[2].z;

         curTri->normal0.x = node->geometry[i]->pNorms[0].x; curTri->normal0.y = node->geometry[i]->pNorms[0].y; curTri->normal0.z = node->geometry[i]->pNorms[0].z;
         curTri->normal1.x = node->geometry[i]->pNorms[1].x; curTri->normal1.y = node->geometry[i]->pNorms[1].y; curTri->normal1.z = node->geometry[i]->pNorms[1].z;
         curTri->normal2.x = node->geometry[i]->pNorms[2].x; curTri->normal2.y = node->geometry[i]->pNorms[2].y; curTri->normal2.z = node->geometry[i]->pNorms[2].z;

         currentTriangle++;
         curTri = &node->triangles[currentTriangle];

         /// Triangle 2
         curTri->vertex0.x = node->geometry[i]->pVertices[2].x; curTri->vertex0.y = node->geometry[i]->pVertices[2].y; curTri->vertex0.z = node->geometry[i]->pVertices[2].z;
         curTri->vertex1.x = node->geometry[i]->pVertices[3].x; curTri->vertex1.y = node->geometry[i]->pVertices[3].y; curTri->vertex1.z = node->geometry[i]->pVertices[3].z;
         curTri->vertex2.x = node->geometry[i]->pVertices[0].x; curTri->vertex2.y = node->geometry[i]->pVertices[0].y; curTri->vertex2.z = node->geometry[i]->pVertices[0].z;

         curTri->normal0.x = node->geometry[i]->pNorms[2].x; curTri->normal0.y = node->geometry[i]->pNorms[2].y; curTri->normal0.z = node->geometry[i]->pNorms[2].z;
         curTri->normal1.x = node->geometry[i]->pNorms[3].x; curTri->normal1.y = node->geometry[i]->pNorms[3].y; curTri->normal1.z = node->geometry[i]->pNorms[3].z;
         curTri->normal2.x = node->geometry[i]->pNorms[0].x; curTri->normal2.y = node->geometry[i]->pNorms[0].y; curTri->normal2.z = node->geometry[i]->pNorms[0].z;

         currentTriangle++;
         break;
      }
   }

   assert(node->nTriangles == currentTriangle);

   delete[] node->geometry;
   node->geometry = NULL;
}

void
C_BspNode::CleanUpPointSet(C_BspNode* node , vector<C_Vertex>& points, bool testBbox)
{
   unsigned int cPoint = 0;

   /// Remove points outside the bbox
   while(cPoint < points.size() && testBbox) {
      if(node->bbox->IsInside(&points[cPoint]) == false) {
         points.erase(points.begin() + cPoint);
         cPoint--;
      }
      cPoint++;
   }

   /// Remove points coinciding with the triangles of the given node
   /// NOTE: VERY BRUTE FORCE WAY. MUST FIND SOMETHING FASTER.
   for(int cTri = 0 ; cTri < node->nTriangles ; cTri++) {
      cPoint = 0;
      while(cPoint < points.size()) {
         if(PointInTriangle(&points[cPoint] , &node->triangles[cTri])) {
            points.erase(points.begin() + cPoint);
            cPoint--;
         }

         cPoint++;
      }
   }
}

void
C_BspNode::DistributeSamplePoints(C_BspNode* node , vector<C_Vertex>& points)
{
   // CLEAN UP POINTS
   CleanUpPointSet(node, points, true);

   if(node->isLeaf) {
      node->pointSet = points;
   } else {
      float dist;
      vector<C_Vertex> frontPoints, backPoints;

      /// Fill in node's pointset
      node->DistributePointsAlongPartitionPlane();
      frontPoints = node->pointSet;
      backPoints = node->pointSet;

      for(unsigned int i = 0 ; i < points.size() ; i++) {
         dist = node->partitionPlane.distanceFromPoint(&points[i]);

         if(FLOAT_GREATER(dist, 0.0f)) {
            frontPoints.push_back(points[i]);
         } else if(FLOAT_GREATER(0.0f, dist)) {
            backPoints.push_back(points[i]);
         } else {
            frontPoints.push_back(points[i]);
            backPoints.push_back(points[i]);
         }
      }

      C_BspNode::DistributeSamplePoints(node->backNode , backPoints);
      C_BspNode::DistributeSamplePoints(node->frontNode , frontPoints);
   }
}

void
C_BspNode::DistributePointsAlongPartitionPlane(void)
{
   C_Vertex min, max, tmp;
   float maxU, maxV, minU, minV;
   float tmpU, tmpV;
   maxU = maxV = SMALLEST_FLOAT;
   minU = minV = GREATEST_FLOAT;
   min.x = min.y = min.z = GREATEST_FLOAT;
   max.x = max.y = max.z = SMALLEST_FLOAT;

   /// Find where the node's partition plane intersects with the node's bounding box
   vector<C_Vertex> intersectionPoints = FindBBoxPlaneIntersections(bbox, &partitionPlane);

   for(USHORT i = 0 ; i < intersectionPoints.size() ; i++) {
      CalculateUV(&partitionPlane, &intersectionPoints[i], &tmpU, &tmpV);

      if(tmpU > maxU) maxU = tmpU;
      if(tmpV > maxV) maxV = tmpV;
      if(tmpU < minU) minU = tmpU;
      if(tmpV < minV) minV = tmpV;
   }

   assert(!FLOAT_EQ(maxU, minU));
   assert(!FLOAT_EQ(maxV, minV));
   float stepU = (maxU - minU) / (float)NPOINTS_U;
   float stepV = (maxV - minV) / (float)NPOINTS_V;

   C_Vector3 P;
   C_Vector3 A(&partitionPlane.points[0]);
   C_Vector3 B(&partitionPlane.points[1]);
   C_Vector3 C(&partitionPlane.points[2]);
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

	pointShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&globalModelviewMatrix.m[0][0]);
 	pointShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

   glEnableVertexAttribArray(pointShader->verticesAttribLocation);
   glVertexAttribPointer(pointShader->verticesAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, &pointSet[0]);
   glDrawArrays(GL_POINTS, 0, n);
   shaderManager->popShader();
}
