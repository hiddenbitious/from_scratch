#ifndef _BSPCOMMON_H_
#define _BSPCOMMON_H_

#include "glsl/glsl.h"
#include "globals.h"
#include "plane.h"
#include "bbox.h"
#include "mesh.h"

#include <vector>

class C_BspNode;
class C_BspTree;

#define BACK		   0
#define FRONT		   1
#define INTERSECTS	2
#define COINCIDENT	3

extern int polyCount;
extern int leavesDrawn;
extern int nodesDrawn;
extern int nConvexRooms;

extern vector<C_Vertex> debug;

struct poly_t {
	C_Vertex* pVertices;
	C_Vertex* pNorms;
	int nVertices;
	bool usedAsDivider;
};

struct brush_t {
	poly_t* pPolys;
	int nPolys;
};

typedef struct {
   C_MeshGroup *mesh;
   ESMatrix matrix;
   unsigned int meshID;
} staticTreeObject_t;

#endif
