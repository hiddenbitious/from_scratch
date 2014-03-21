#ifndef _BSPHELPERFUNCTIONS_H_
#define _BSPHELPERFUNCTIONS_H_

#include "bspNode.h"
#include "bspTree.h"

void CalculateUV(C_Plane* plane , C_Vertex* P , float* u , float* v);
vector<C_Vertex> FindBBoxPlaneIntersections(C_BBox* bbox, C_Plane* plane);

/// If we know that the line formed from ptA and ptB intersects the plane
/// this function finds the intersection point and returns it.
C_Vertex FindIntersectionPoint(C_Vertex *ptA , C_Vertex *ptB , C_Plane *plane);
bool FindIntersectionPoint_withCheck(C_Vertex *ptA, C_Vertex *ptB, C_Plane *plane, C_Vertex* intePoint);
bool PointInTriangle(C_Vertex* points, triangle_vn *triangle);
bool RayTriangleIntersection(C_Vertex* p1 , C_Vertex* p2 , triangle_vn *triangle);
/// Tests a vertex against a plane whether it is in FRONT, BACK or COINCIDENT
int ClassifyVertex(C_Plane *plane, C_Vertex *vertex);
/// Tests if polygon is in front, back, coincident or it spans the given plane
int ClassifyPolygon(C_Plane *plane, poly_t *polygon);
/// Splits the given polygon in two new polygon. The polygon must be spanning the plane given
void SplitPolygon(C_Plane *plane, poly_t *polygon, poly_t **front, poly_t **back);

#endif
