#ifndef _BSPTREE_H_
#define _BSPTREE_H_

#include "globals.h"
#include "plane.h"
#include "trimesh.h"
#include "bspCommon.h"
#include "map.h"

#include <vector>
#include <iostream>

using namespace std;

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

   private:
      C_Map *map;

	public:
		C_BspTree(USHORT depth);
		~C_BspTree();

		/// Read geometry file
		bool ReadGeometryFile(const char* fileName);

		/// Read a map created with the editor and generate the geometry
		bool ReadMapFile(const char* fileName);

		void WritePVSFile(const char *fileName);
		bool ReadPVSFile(const char *fileName);

		void BuildBspTree(void);
		void BuildPVS(void);

		void TraceVisibility(void);
		bool CheckVisibility(C_BspNode *node1 , C_BspNode *node2);
		bool RayIntersectsSomethingInTree(C_BspNode *node , C_Vertex *start , C_Vertex *end);

		void TessellatePolygons(void);

		void Draw(void);
		int Draw2(C_Vector3* cameraPosition);
		void Draw3(void);
		int Draw_PVS(C_Vector3* cameraPosition);

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
};

#endif
