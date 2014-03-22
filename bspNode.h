#ifndef _C_BSPNODE_H_
#define _C_BSPNODE_H_

#include "bspCommon.h"

class C_BspNode {
friend class C_BspTree;

protected:

   vector<C_Vertex> pointSet;
   vector<C_BspNode*> connectedLeaves;
   bool drawn;

   vector<staticTreeObject_t *> staticObjects;
   bool insertStaticObject(staticTreeObject_t *mesh, C_Vertex *point);

public:
   /// Node's ID
   ULONG nodeID;

   vector<C_BspNode*> PVS;


   USHORT depth;
   bool *visibleFrom;
   bool *checkedVisibilityWith;

   /// Plane used to classify the polygons
   C_Plane partitionPlane;

   /// If this node is leaf
   bool isLeaf;

   /// Number of polys in node
   int nPolys;

   /// Actual polygon data
   poly_t** geometry;

   /// Same poly tesselated into tirangles
   int nTriangles;
   triangle_vn *triangles;

   /// Children nodes
   C_BspNode *frontNode;
   C_BspNode *backNode;
   C_BspNode *fatherNode;
   C_BspTree *tree;

   /// Node's bounding box.
   C_BBox bbox;

   /// Algorithm that chooses a polygon from a given geometry set and returns it as a splitting plane
   bool SelectPartitionfromList(C_Plane* finalPlane);

   bool IsConvex(void);

   C_BspNode();
   C_BspNode(poly_t** geom , int nPolys);
   ~C_BspNode();

   /// Recursively builds the tree
   void BuildBspTree(C_BspTree* tree);

   /// Calculate node's bbox
   void CalculateBBox(void);

   void DistributePointsAlongPartitionPlane(void);
   void DistributeSamplePoints(vector<C_Vertex>& points);
   void CleanUpPointSet(vector<C_Vertex>& points, bool testWithBbox, bool testWithGeometry);
   bool addNodeToPVS(C_BspNode *node);

   void DrawPointSet(void);


   void Draw(C_Camera *camera, C_BspTree* tree, bool usePVS);
   //		static void Draw_PVS(C_Vector3* cameraPosition , C_BspNode* node , C_BspTree* tree);
   void Draw(C_Camera *camera);

   /// Brakes down CONVEX polygon of 4 or 5 vertices into triangles.
   /// Also it frees any memory reserved for the previous polygons
   /// (not the data itself but the pointers).
   /// New pointers are stored at the triangles var.
   void TessellatePolygonsInLeaves(void);
};

#endif

