/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

#ifndef _BSPCOMMON_H_
#define _BSPCOMMON_H_

#include <vector>

class C_BspNode;
class C_BspTree;

#define BACK		0
#define FRONT		1
#define INTERSECTS	2
#define COINCIDENT	3

extern int polyCount;
extern int leavesDrawn;
extern int nodesDrawn;
extern int nConvexRooms;

extern vector<C_Vertex> debug;

struct poly {
	C_Vertex* pVertices;
	C_Vertex* pNorms;
	int nVertices;
	bool usedAsDivider;
};

struct brush {
	poly* pPolys;
	int nPolys;
};

#endif
