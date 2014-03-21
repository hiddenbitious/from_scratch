#include "bspHelperFunctions.h"

bool
PointInTriangle(C_Vertex* point , triangle_vn *triangle)
{
   C_Plane plane(&triangle->vertex0, &triangle->vertex1, &triangle->vertex2);

	/// This is cheaper. Calculate it first.
	if(!FLOAT_EQ(plane.distanceFromPoint(point), 0.0f)) {
		return false;
	}

	float u, v;
	CalculateUV(&plane , point , &u , &v);
	return ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f));
}


bool
RayTriangleIntersection(C_Vertex* p1, C_Vertex* p2, triangle_vn *triangle)
{
   C_Plane plane(&triangle->vertex0, &triangle->vertex1, &triangle->vertex2);

	/// First check ray with plane in case it is parallel to it
	C_Vertex interPoint;
	if(FindIntersectionPoint_withCheck(p1, p2, &plane, &interPoint) == false) {
		return false;
	}

	/// If ray hits plane check if the iPoint is in Triangle
	float u , v;
	CalculateUV(&plane , &interPoint , &u , &v);
	return ((u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f));
}


void
CalculateUV(C_Plane *plane, C_Vertex *point, float *u, float *v)
{
	float dot00, dot01, dot02, dot11, dot12;
	C_Vector3 A(&plane->points[0]);
	C_Vector3 B(&plane->points[1]);
	C_Vector3 C(&plane->points[2]);
	C_Vector3 P(point);

	C_Vector3 v0 = C - A;
	C_Vector3 v1 = B - A;
	C_Vector3 v2 = P - A;

	dot00 = C_Vector3::DotProduct(&v0 , &v0);
	dot01 = C_Vector3::DotProduct(&v0 , &v1);
	dot02 = C_Vector3::DotProduct(&v0 , &v2);
	dot11 = C_Vector3::DotProduct(&v1 , &v1);
	dot12 = C_Vector3::DotProduct(&v1 , &v2);

	float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
	*u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	*v = (dot00 * dot12 - dot01 * dot02) * invDenom;
}


// The intersection polygon can vary from 3 to 6 vertices
// I will consider the bbox as 12 lines with which i will test the plane for intersection
vector<C_Vertex>
FindBBoxPlaneIntersections(C_BBox *bbox, C_Plane *plane)
{
	vector<C_Vertex> points;
	C_Vertex tmpPoint, bboxVertices[8];

	bbox->GetVertices(bboxVertices);

/*
	int i;
// BOTTOM
   for(i = 0; i < 4; i++) {
	if(FindIntersectionPoint_withCheck(&bboxVertices[(i)%4] , &bboxVertices[(i+1)%4] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }
   }

// TOP
   for(i = 0; i < 4; i++) {
      if(FindIntersectionPoint_withCheck(&bboxVertices[((i + 4)%4) + 4] , &bboxVertices[((i+5)%4) + 4] , plane , &tmpPoint))
         if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }
   }
*/

// BOTTOM
	if(FindIntersectionPoint_withCheck(&bboxVertices[0] , &bboxVertices[1] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[1] , &bboxVertices[2] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint))	{ points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[2] , &bboxVertices[3] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[3] , &bboxVertices[0] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

// TOP
	if(FindIntersectionPoint_withCheck(&bboxVertices[4] , &bboxVertices[5] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[5] , &bboxVertices[6] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[6] , &bboxVertices[7] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

	if(FindIntersectionPoint_withCheck(&bboxVertices[7] , &bboxVertices[4] , plane , &tmpPoint))
		if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }

// CIRCUMFERENCE
	for(USHORT i = 0 ; i < 4 ; i++) {
		if(FindIntersectionPoint_withCheck(&bboxVertices[i] , &bboxVertices[i + 4] , plane , &tmpPoint))
			if(bbox->IsInside(&tmpPoint)) { points.push_back(tmpPoint); }
	}

	return points;
}


C_Vertex
FindIntersectionPoint(C_Vertex *ptA , C_Vertex *ptB , C_Plane *plane)
{
	C_Vertex intersectionPoint, planeNormal;

	C_Vertex v;
	v.x = ptB->x - ptA->x;
	v.y = ptB->y - ptA->y;
	v.z = ptB->z - ptA->z;
	planeNormal = plane->getPlaneNormal();
	float sect = -plane->distanceFromPoint(ptA) / C_Vector3::DotProduct(&planeNormal , &v);
	intersectionPoint.x = ptA->x + v.x * sect;
	intersectionPoint.y = ptA->y + v.y * sect;
	intersectionPoint.z = ptA->z + v.z * sect;

	return intersectionPoint;
}


bool
FindIntersectionPoint_withCheck(C_Vertex *ptA, C_Vertex *ptB, C_Plane *plane, C_Vertex *intersectionPoint)
{
	C_Vertex v, planeNormal;

	v.x = ptB->x - ptA->x;
	v.y = ptB->y - ptA->y;
	v.z = ptB->z - ptA->z;
	planeNormal = plane->getPlaneNormal();
	float denominator = C_Vector3::DotProduct(&planeNormal, &v);

	/// If denominator is 0 then line and plane are parallel so they don't intersect
	if(FLOAT_EQ(denominator, 0.0f))
	   return false;

	float sect = -plane->distanceFromPoint(ptA) / denominator;
	intersectionPoint->x = ptA->x + v.x * sect;
	intersectionPoint->y = ptA->y + v.y * sect;
	intersectionPoint->z = ptA->z + v.z * sect;

	return true;
}

int
ClassifyVertex(C_Plane *plane , C_Vertex *vertex)
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
ClassifyPolygon(C_Plane *plane, poly_t *polygon)
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

void
SplitPolygon(C_Plane *plane , poly_t *polygon , poly_t **front , poly_t **back)
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
}
