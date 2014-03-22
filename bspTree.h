#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#include <iostream>

#include "bspCommon.h"

using namespace std;

typedef struct {
   /// Drawing / geometry statistics
   int leavesDrawn;
   int totalLeaves;
   int staticObjectsDrawn;
   int totalStaticObjects;
   int trianglesDrawn;
   int totalTriangles;
} treeDrawStatistics_t;

/// Tree statistics
typedef struct {
   int nLeaves;
   int nNodes;
   int nSplits;
   int maxDepth;
   int depthReached;
   int nInitialPolys;
   int nFinalPolys;
} treeStatistics_t;

class C_BspTree {
friend class C_BspNode;

public:
   C_BspNode* headNode;

   brush_t* pBrushes;
   int nBrushes;
   int nPolys;
   C_Vertex position;

   /// Scale the map
   float scaleFactor;

   /// Calculate normal vectors
   void CalcNorms(void);

   vector<poly_t*> tessellatedPolys;
   poly_t** pRawPolys;

   USHORT leafToDraw;
   USHORT nNodesToDraw;

   treeDrawStatistics_t statistics;
   treeStatistics_t treeStats;
public:
   C_BspTree(USHORT depth);
   ~C_BspTree();

   /// Read geometry file
   bool ReadGeometryFile(const char* fileName);

   void WritePVSFile(const char *fileName);
   bool ReadPVSFile(const char *fileName);

   void BuildBspTree(void);
   void BuildPVS(void);

   void TraceVisibility(void);
   C_BspNode *CheckVisibility(C_BspNode *node1 , C_BspNode *node2);
   C_BspNode *RayIntersectsSomethingInTree(C_BspNode *node , C_Vertex *start , C_Vertex *end);
   void insertStaticObject(C_MeshGroup *mesh, ESMatrix *matrix);

   void TessellatePolygons(void);

   void Draw(void);
   int Draw2(C_Camera *camera);
   void Draw3(void);
   int Draw_PVS(C_Camera *camera);

   /// Max depth allowed
   USHORT maxDepth;
   /// Number of polygon splits happen while building the tree
   ULONG nSplits;

   /// Limits reached
   int depthReached;
   int lessPolysInNodeFound;

   USHORT nLeaves;
   USHORT nNodes;
   USHORT nConvexRooms;

   /// Keep all the leaves for easy reference
   vector<C_BspNode*> leaves;

   void FindConnectedLeaves(void);

   void IncreaseLeavesDrawn(); // { if ( leafToDraw < nLeaves ) leafToDraw++; cout << leafToDraw << endl;}
   void DecreaseLeavesDrawn(); // { if ( leafToDraw > 0 ) leafToDraw--; cout << leafToDraw << endl;}

   void IncreaseNodesDrawn(); // { if ( leafToDraw < nLeaves ) leafToDraw++; cout << leafToDraw << endl;}
   void DecreaseNodesDrawn(); // { if ( leafToDraw > 0 ) leafToDraw--; cout << leafToDraw << endl;}

   inline USHORT GetnLeavesToDraw() { return leafToDraw; }
   inline USHORT GetnNodesToDraw() { return nNodesToDraw; }

   void dumpSamplePoints(const char *filename);

   C_BBox bbox;

   /**
    * Detect holes in the map (void space that does not belong to any leaf)
    * and close them by merging leaves
    */
   void closeLeafHoles(void);

   void insertMeshIntoTree(C_MeshGroup *mesh);
};

#endif
