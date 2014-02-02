#include "bspTree.h"
#include "bspNode.h"
#include "vectors.h"
#include "bspHelperFunctions.h"

#include <fstream>
#include <GL/gl.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int polyCount;
int leavesDrawn;
int nodesDrawn;
int nConvexRooms;

const static float tileSize = 20.0f;

C_BspTree::C_BspTree(USHORT depth)
{
	nBrushes = 0;
	nPolys = 0;
	scaleFactor = 10.0f;
	pBrushes = NULL;
	headNode = NULL;
	pRawPolys = NULL;

	maxDepth = depth;
	lessPolysInNodeFound = INT_MAX;

	nSplits = 0;
	nConvexRooms = 0;

	depthReached = -1;
	nLeaves = 0;
	leafToDraw = 0;
	nNodesToDraw = 0;
	nNodes = 0;
}

C_BspTree::~C_BspTree(void)
{
	// Delete data
	for(int i = 0 ; i < nBrushes ; i++) {
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			delete[] pBrushes[i].pPolys[j].pVertices;
			delete[] pBrushes[i].pPolys[j].pNorms;
		}
		delete[] pBrushes[i].pPolys;
	}

	delete[] pBrushes;
	delete[] pRawPolys;
}


void
C_BspTree::IncreaseLeavesDrawn()
{
	if(leafToDraw < nLeaves - 1) { leafToDraw++; } cout << "Leaf drawn: " << leafToDraw << endl;
}

void
C_BspTree::DecreaseLeavesDrawn()
{
	if(leafToDraw > 0) { leafToDraw--; } cout << "Leaves drawn: " << leafToDraw << endl;
}

void
C_BspTree::IncreaseNodesDrawn()
{
	if(nNodesToDraw < nNodes) { nNodesToDraw++; } cout << "Nodes drawn: " << nNodesToDraw << endl;
}

void
C_BspTree::DecreaseNodesDrawn()
{
	if(nNodesToDraw > 0) { nNodesToDraw--; } cout << "Node drawn: " << nNodesToDraw << endl;
}

bool
C_BspTree::ReadGeometryFile(const char* fileName)
{
	printf("%s\n", __FUNCTION__);

	ifstream file(fileName , ios::in | ios::binary);

	if(!file.is_open()) {
		cout << "Couldn't open \"" << fileName << "\"" << endl;
		return false;
	}

	file.read((char*)&nPolys , sizeof(int));
	cout << nPolys << endl;
	pRawPolys = new poly_t*[nPolys];
	int currentPoly = 0;

	/// Read number of brushes/meshes
	file.read((char*)&nBrushes , sizeof(int));
	cout << nBrushes << endl;

	pBrushes = new brush_t[nBrushes];

	/// For each brush...
	for(int i = 0 ; i < nBrushes ; i++) {
		/// ...read number of polys
		file.read((char*)&pBrushes[i].nPolys , sizeof(int));
		cout << pBrushes[i].nPolys << endl;

		pBrushes[i].pPolys = new poly_t[pBrushes[i].nPolys];

		/// For each poly in brush
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			/// Read number of vertices
			file.read((char*)&pBrushes[i].pPolys[j].nVertices, sizeof(int));

			pBrushes[i].pPolys[j].pVertices = new C_Vertex[pBrushes[i].pPolys[j].nVertices];
			pBrushes[i].pPolys[j].pNorms = new C_Vertex[pBrushes[i].pPolys[j].nVertices];

			pRawPolys[currentPoly] = &pBrushes[i].pPolys[j];
			pRawPolys[currentPoly]->usedAsDivider = false;
			currentPoly++;

			/// Read vertices
			for(int k = 0 ; k < pBrushes[i].pPolys[j].nVertices ; k++) {
				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].x , sizeof(float));
				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].y , sizeof(float));
				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].z , sizeof(float));

				pBrushes[i].pPolys[j].pVertices[k].x /= scaleFactor;
				pBrushes[i].pPolys[j].pVertices[k].y /= scaleFactor;
				pBrushes[i].pPolys[j].pVertices[k].z /= scaleFactor;
			}
//			TessellatePolygon ( &pBrushes[i].pPolys[j] );
		}
	}
	file.close();

	assert(currentPoly == nPolys);

	CalcNorms();
	return true;
}

void
C_BspTree::Draw(void)
{
	for(int np = 0 ; np < nPolys ; np++) {
		glBegin(GL_POLYGON);
		for(int k = 0 ; k < pRawPolys[np]->nVertices ; k++) {
			glNormal3f(pRawPolys[np]->pNorms[k].x , pRawPolys[np]->pNorms[k].y , pRawPolys[np]->pNorms[k].z);
			glVertex3f(pRawPolys[np]->pVertices[k].x , pRawPolys[np]->pVertices[k].y , pRawPolys[np]->pVertices[k].z);

			//glNormal3f ( rawPolys[np].pNorms[k].x , rawPolys[np].pNorms[k].y , rawPolys[np].pNorms[k].z );
			//glVertex3f ( rawPolys[np].pVertices[k].x , rawPolys[np].pVertices[k].y , rawPolys[np].pVertices[k].z );
		}
		glEnd();
	}
}

void
C_BspTree::CalcNorms(void)
{
	printf("%s\n", __FUNCTION__);
	C_Vector3 norm;

	// For each brush
	for(int i = 0 ; i < nBrushes ; i++) {
		// For each poly in brush
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			norm = C_Vector3::CrossProduct2(&pBrushes[i].pPolys[j].pVertices[0] , &pBrushes[i].pPolys[j].pVertices[1] , &pBrushes[i].pPolys[j].pVertices[2]);
			norm.Normalize();
			for(int k = 0 ; k < pBrushes[i].pPolys[j].nVertices ; k++) {
				pBrushes[i].pPolys[j].pNorms[k].x = norm.x;
				pBrushes[i].pPolys[j].pNorms[k].y = norm.y;
				pBrushes[i].pPolys[j].pNorms[k].z = norm.z;
			}
//			rawPolys.push_back ( pBrushes[i].pPolys[j] );
		}
	}
}

void
C_BspTree::BuildPVS(void)
{
	printf("%s\n", __FUNCTION__);

	/// An iparhei arheio me tin pliroforia diabase apo ekei
	bool pvsFileFound = false;
	pvsFileFound = this->ReadPVSFile("pvs.txt");

	cout << "Building PVS..." << endl;
	cout << "\tDistributing sample points..." << flush;
	C_BspNode::DistributeSamplePoints(headNode , headNode->pointSet);
	cout << "Done!" << endl;

	if(pvsFileFound) {
		cout << "Found in file. Skipping calculations." << endl;
	} else {
		C_BspTree::TraceVisibility();
		cout << "Done!" << endl << endl;
	}

	cout << "Done!" << endl << endl;

	/// Write PVS into a file
	if(!pvsFileFound) {
		WritePVSFile("pvs.txt");
	}

//   exit(0);
}

typedef struct {
   int tid;
   C_BspTree *tree;
} threadData_t;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static float progress;

void *TraceVisibility_Thread(void *data_)
{
   threadData_t *data = (threadData_t *)data_;
   int tid = data->tid, s1;
   C_BspTree *tree = data->tree;
   vector<C_BspNode *> &leaves = tree->leaves;
   C_BspNode *leaf1, *leaf2, *leaf3;

   bool res;
   int load = leaves.size();
   int start = load / MAX_THREADS * tid;
   int end = load / MAX_THREADS * (tid + 1);
   float step = 20.0f / (float)load;

   /// thread 0 prints progress bar
   if(!tid) {
      pthread_mutex_lock(&mutex);
      progress = step;
      pthread_mutex_unlock(&mutex);

      cout << "\n\t\t0%|---------50---------|100%\n\t   ";
   }

	for(unsigned int l1 = start; l1 < end; l1++) {
	   leaf1 = leaves[l1];
		for(unsigned int l2 = 0; l2 < leaf1->PVS.size(); l2++) {
		   leaf2 = leaf1->PVS[l2];
         s1 = leaf2->PVS.size();
         for(unsigned int l3 = 0; l3 < s1; l3++) {
            leaf3 = leaf2->PVS[l3];

            /// Sanity checks
            assert(leaf1->isLeaf);
            assert(leaf2->isLeaf);
            assert(leaf3->isLeaf);

            if(leaf1->nodeID == leaf3->nodeID) {
               continue;
            }

            pthread_mutex_lock(&mutex);
            assert(leaf1->checkedVisibilityWith[leaf3->nodeID] == leaf3->checkedVisibilityWith[leaf1->nodeID]);
            assert(leaf1->visibleFrom[leaf3->nodeID] == leaf3->visibleFrom[leaf1->nodeID]);
            if(leaf1->visibleFrom[leaf3->nodeID] || leaf1->checkedVisibilityWith[leaf3->nodeID]) {
               pthread_mutex_unlock(&mutex);
               continue;
            }
            pthread_mutex_unlock(&mutex);

            res = tree->CheckVisibility(leaf1, leaf3);

            pthread_mutex_lock(&mutex);
            if(res) {
               leaf1->PVS.push_back(leaf3);
               leaf1->visibleFrom[leaf3->nodeID] = true;

               leaf3->PVS.push_back(leaf1);
               leaf3->visibleFrom[leaf1->nodeID] = true;
            }

            leaf1->checkedVisibilityWith[leaf3->nodeID] = true;
            leaf3->checkedVisibilityWith[leaf1->nodeID] = true;
            pthread_mutex_unlock(&mutex);
         }
      }

		pthread_mutex_lock(&mutex);
  		progress += step;
  		pthread_mutex_unlock(&mutex);

  		if(!tid) {
         cout << "\r\t\t   ";
         for(int k = 0 ; k < (int)progress ; k++)
            cout << "*" << flush;
      }
	}

	pthread_exit(NULL);
	return NULL;
}

void
C_BspTree::TraceVisibility(void)
{
   pthread_t threads[MAX_THREADS];
   threadData_t threadData[MAX_THREADS];
   int cb, ret, i, j;
   timeval start, end;
   double elapsedTime;

   assert(nLeaves == leaves.size());

/// Initialize variables
/// ----------------------
	for(i = 0 ; i < nLeaves; i++) {
		leaves[i]->visibleFrom = new bool[nNodes];
		leaves[i]->checkedVisibilityWith = new bool[nNodes];

		memset(leaves[i]->visibleFrom, false, nNodes * sizeof(bool));
		memset(leaves[i]->checkedVisibilityWith, false, nNodes * sizeof(bool));
	}

/// Find connected leaves
/// ----------------------
   printf("\tFinding conected leaves...");
   gettimeofday(&start, NULL);

	for(i = 0; i < nLeaves; i++) {
		for(j = 0; j < nLeaves; j++) {
			if(i == j || leaves[j]->visibleFrom[leaves[i]->nodeID] || leaves[i]->visibleFrom[leaves[j]->nodeID])
				continue;

         /// Loop through point sets
			for(unsigned int p1 = 0; p1 < leaves[i]->pointSet.size(); p1++) {
				for(unsigned int p2 = 0; p2 < leaves[j]->pointSet.size(); p2++) {
					if(FLOAT_EQ(leaves[i]->pointSet[p1].x, leaves[j]->pointSet[p2].x) &&
                  FLOAT_EQ(leaves[i]->pointSet[p1].y, leaves[j]->pointSet[p2].y) &&
						FLOAT_EQ(leaves[i]->pointSet[p1].z, leaves[j]->pointSet[p2].z)) {
						if(leaves[j]->visibleFrom[leaves[i]->nodeID] == false) {
							leaves[j]->connectedLeaves.push_back(leaves[i]);
							leaves[j]->PVS.push_back(leaves[i]);
							leaves[j]->visibleFrom[leaves[i]->nodeID] = true;
						}

						if(leaves[i]->visibleFrom[leaves[j]->nodeID] == false) {
							leaves[i]->connectedLeaves.push_back(leaves[j]);
							leaves[i]->PVS.push_back(leaves[j]);
							leaves[i]->visibleFrom[leaves[j]->nodeID] = true;
						}

						p1 = leaves[i]->pointSet.size();
						break;
					}
				}
			}
		}
	}

   gettimeofday(&end, NULL);
   elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
   elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms
   printf("Done! (%.2f ms)\n", elapsedTime);

/// Trace visibility
/// ----------------------
   printf("\tTracing Visibility...\n");
   gettimeofday(&start, NULL);

	for(cb = 0; cb < MAX_THREADS; cb++) {
		threadData[cb].tid = cb;
		threadData[cb].tree = this;
		printf("\t\t[Creating thread %d]\n", cb);
      /// Fork threads
		ret = pthread_create(&threads[cb], NULL, TraceVisibility_Thread, (void *)&threadData[cb]);
		if(ret) {
		   printf("Could not create new thread.\n");
		   abort();
      }
	}

   /// Join threads here
	for(cb = 0; cb < MAX_THREADS; cb++) {
		pthread_join(threads[cb], NULL);
	}

   gettimeofday(&end, NULL);
   elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
   elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms

   printf("\n\nDone (%.2f ms)\n", elapsedTime);

	pthread_mutex_destroy(&mutex);
}

bool
C_BspTree::CheckVisibility(C_BspNode *node1, C_BspNode *node2)
{
	for(unsigned int p1 = 0; p1 < node1->pointSet.size(); p1++) {
		for(unsigned int p2 = 0; p2 < node2->pointSet.size(); p2++) {
			if(C_BspTree::RayIntersectsSomethingInTree(headNode, &node1->pointSet[p1], &node2->pointSet[p2]) == false) {
				return true;
			}
		}
	}
	return false;
}

bool
C_BspTree::RayIntersectsSomethingInTree(C_BspNode *node, C_Vertex *start, C_Vertex *end)
{
	if(node->isLeaf) {
		for(int cp = 0 ; cp < node->nTriangles ; cp++) {
			if(RayTriangleIntersection(start , end , &node->triangles[cp])) {
				return true;
			}
		}
		return false;
	}

	int startSide = C_BspNode::ClassifyVertex(&node->partitionPlane , start);
	int endSide = C_BspNode::ClassifyVertex(&node->partitionPlane , end);

   /// If the ray spans the node's partition plane, then send the beam down both sides of node
	if((startSide == COINCIDENT && endSide == COINCIDENT) ||
      (startSide != endSide && startSide != COINCIDENT && endSide != COINCIDENT)) {
		if(C_BspTree::RayIntersectsSomethingInTree(node->backNode , start , end)) {
			return true;
		}
		if(C_BspTree::RayIntersectsSomethingInTree(node->frontNode , start , end)) {
			return true;
		}
	}

   /// If beam is whole in front of partition plane the send it down the front node
	if(startSide == FRONT || endSide == FRONT) {
		if(C_BspTree::RayIntersectsSomethingInTree(node->frontNode , start , end)) {
			return true;
		}
	}

   /// Respectively send it down to back node
	if(startSide == BACK || endSide == BACK) {
		if(C_BspTree::RayIntersectsSomethingInTree(node->backNode , start , end)) {
			return true;
		}
	}

	return false;
}

void
C_BspTree::BuildBspTree(void)
{
	cout << "***********************************************" << endl;
	cout << "Building bsp tree...";

//	ULONG start = timeGetTime ();

	headNode = new C_BspNode(pRawPolys , nPolys);
	C_BspNode::BuildBspTree(headNode , this);

	TessellatePolygons();

//	ULONG time = timeGetTime () - start;

	cout << "Done!" << endl;

	// Print out statistics
	cout << "\tTotal polygons in tree before splits: " << nPolys << endl;
	cout << "\tPolygons splitted: " << nSplits << endl;
	cout << "\tTotal polygons in tree after splits: " << nPolys + nSplits << endl;
	cout << "\tNumber of leaves in tree: " << nLeaves << endl;
	cout << "\tNumber of nodes in tree: " << nNodes << endl;
	cout << "\tMinimun number of polys assigned in node: " << lessPolysInNodeFound << endl;
	cout << "\tMaximum depth allowed: " << maxDepth << endl;
	cout << "\tDepth reached: " << depthReached << endl;
//	cout << "Time elapsed: " << (float)(time/1000.0f) << " seconds." << endl;
	cout << "***********************************************\n\n" << endl;
}

int
C_BspTree::Draw2(C_Vector3* cameraPosition)
{
	polyCount = 0;
	leavesDrawn = 0;
	nodesDrawn = 0;

   /// Pass matrices to shader
	/// Keep a copy of global movelview matrix
	shaderManager->pushShader(bspShader);
   glEnableVertexAttribArray(bspShader->verticesAttribLocation);
	glEnableVertexAttribArray(bspShader->normalsAttribLocation);

	ESMatrix mat = globalModelviewMatrix;
	esTranslate(&mat, position.x , position.y , position.z);

	bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
	bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);
	C_BspNode::Draw(cameraPosition, headNode, this);
	glFlush();
	shaderManager->popShader();

	return polyCount;
}

void
C_BspTree::Draw3(void)
{
	shaderManager->pushShader(bspShader);
   glEnableVertexAttribArray(bspShader->verticesAttribLocation);
	glEnableVertexAttribArray(bspShader->normalsAttribLocation);

	ESMatrix mat = globalModelviewMatrix;
	esTranslate(&mat, position.x , position.y , position.z);

	bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
	bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

	C_BspNode::Draw(NULL, leaves[leafToDraw], this);

   if(drawConnectedToo) {
      for(unsigned int j = 0 ; j < leaves[leafToDraw]->connectedLeaves.size() ; j++) {
         C_BspNode::Draw(NULL, leaves[leafToDraw]->connectedLeaves[j], this);
   //		leaves[leafToDraw]->connectedLeaves[j]->Draw(bspShader);
      }
   }

	glFlush();
	shaderManager->popShader();
}

int
C_BspTree::Draw_PVS(C_Vector3* cameraPosition)
{
	polyCount = 0;
	leavesDrawn = 0;
	nodesDrawn = 0;

   /// Set all leaves as non drawn
	for(unsigned int i = 0 ; i < leaves.size() ; i++) {
		leaves[i]->drawn = false;
	}

   /// Pass matrices to shader
	/// Keep a copy of global movelview matrix
	shaderManager->pushShader(bspShader); {
      glEnableVertexAttribArray(bspShader->verticesAttribLocation);
      glEnableVertexAttribArray(bspShader->normalsAttribLocation);
      ESMatrix mat = globalModelviewMatrix;
      esTranslate(&mat, position.x , position.y , position.z);

      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);

      glFrontFace(GL_CW);
      C_BspNode::Draw_PVS(cameraPosition, headNode, this);
      glFrontFace(GL_CCW);
   }
   shaderManager->popShader();

	return polyCount;
}

void
C_BspTree::TessellatePolygons(void)
{
	C_BspNode::TessellatePolygonsInLeaves(headNode);
}

void
C_BspTree::WritePVSFile(const char *fileName)
{
	fstream filestr;
	filestr.open("pvs.txt" , fstream::out);

	filestr << nLeaves << endl;

	for(int i = 0; i < nLeaves; i++) {
		filestr << leaves[i]->nodeID << " * " << leaves[i]->PVS.size();

		for(unsigned int j = 0; j < leaves[i]->PVS.size(); j++) {
			filestr << " " << leaves[i]->PVS[j]->nodeID;
		}

		filestr << endl;
	}

	filestr.close();
}

bool
C_BspTree::ReadPVSFile(const char *filename)
{
	fstream filestr;
	filestr.open(filename , fstream::in);

	if(filestr.fail()) {
		return false;
	}

	filestr >> nLeaves;

	for(int i = 0; i < nLeaves; i++) {
		ULONG nodeId;
		int size;

		filestr >> nodeId;
		filestr.ignore(3);
		filestr >> size;

		for(int j = 0; j < size; j++) {
			filestr >> nodeId;

			int k = 0;
			while(nodeId != leaves[k++]->nodeID);
			k--;
			leaves[i]->PVS.push_back(leaves[k]);
		}
	}

	return true;
}

bool
C_BspTree::ReadMapFile(const char *filename)
{
   if(!readMap(filename))
      return false;

   /// Count walls in map
   int nWalls = 0;
   for(int x = 0; x < TILES_ON_X; x++) {
      for(int y = 0; y < TILES_ON_Y; y++) {
         nWalls += tiles[x][y].getType() == 1;
      }
   }

   printf("%s: nWalls: %d nPolys: %d\n", __FUNCTION__, nWalls, nWalls * 5);

   /// Allocate raw polys
   /// 4 faces for each wall + 1 for top/roof
   nPolys = nWalls * 5;
   pRawPolys = new poly_t *[nPolys];
   /// Use just one brush
   nBrushes = 1;
   pBrushes = new brush_t[1];
   pBrushes[0].nPolys = nPolys;
   pBrushes[0].pPolys = new poly_t[nPolys];
   int wall = 0;
   C_TexCoord center;

   /// Generate wall geometry
   for(int x = 0; x < TILES_ON_X; x++) {
      for(int y = 0; y < TILES_ON_Y; y++) {
         if(tiles[x][y].getType() != 1) {
            continue;
         }

         /// Generate vertices from tile coordinates
         center.u = x * tileSize + tileSize / 2.0f;
         center.v = y * tileSize + tileSize / 2.0f;

         /// Left wall
         pBrushes[0].pPolys[wall].nVertices = 4;
         pBrushes[0].pPolys[wall].pVertices = new C_Vertex[4];
         pBrushes[0].pPolys[wall].pNorms = new C_Vertex[4];

         pRawPolys[wall] = &pBrushes[0].pPolys[wall];
			pRawPolys[wall]->usedAsDivider = false;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[1].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[2].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[2].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[2].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[3].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[3].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[3].z = center.v - tileSize / 2.0f;
         ++wall;

         /// Front wall
         pBrushes[0].pPolys[wall].nVertices = 4;
         pBrushes[0].pPolys[wall].pVertices = new C_Vertex[4];
         pBrushes[0].pPolys[wall].pNorms = new C_Vertex[4];

         pRawPolys[wall] = &pBrushes[0].pPolys[wall];
			pRawPolys[wall]->usedAsDivider = false;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[1].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;
         ++wall;

         /// Right wall
         pBrushes[0].pPolys[wall].nVertices = 4;
         pBrushes[0].pPolys[wall].pVertices = new C_Vertex[4];
         pBrushes[0].pPolys[wall].pNorms = new C_Vertex[4];

         pRawPolys[wall] = &pBrushes[0].pPolys[wall];
			pRawPolys[wall]->usedAsDivider = false;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[1].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;
         ++wall;

         /// Back wall
         pBrushes[0].pPolys[wall].nVertices = 4;
         pBrushes[0].pPolys[wall].pVertices = new C_Vertex[4];
         pBrushes[0].pPolys[wall].pNorms = new C_Vertex[4];

         pRawPolys[wall] = &pBrushes[0].pPolys[wall];
			pRawPolys[wall]->usedAsDivider = false;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[1].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = -tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v - tileSize / 2.0f;
         ++wall;

         /// Top/roof wall
         pBrushes[0].pPolys[wall].nVertices = 4;
         pBrushes[0].pPolys[wall].pVertices = new C_Vertex[4];
         pBrushes[0].pPolys[wall].pNorms = new C_Vertex[4];

         pRawPolys[wall] = &pBrushes[0].pPolys[wall];
			pRawPolys[wall]->usedAsDivider = false;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[1].x = center.u + tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[1].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v - tileSize / 2.0f;

         pBrushes[0].pPolys[wall].pVertices[0].x = center.u - tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].y = tileSize / 2.0f;
         pBrushes[0].pPolys[wall].pVertices[0].z = center.v + tileSize / 2.0f;
         ++wall;
      }
   }

   assert(wall == 5 * nWalls);

	CalcNorms();
   return true;
}

bool
C_BspTree::readMap(const char *fileName)
{
	int _xTiles, _yTiles, _tileSize, tmp, nTiles, _x, _y;
	int i, c;
	char buf[MAX_PARAMETER_LENGTH];

	FILE *fd;

	if((fd = fopen(fileName, "r")) == NULL)
		return false;

	fscanf(fd, "%d", &_xTiles);		/// Read size on x.
	fscanf(fd, "%d", &_yTiles);		/// Read size on y.
	fscanf(fd, "%d", &nTiles);		   /// Read number of tiles stored in file.
	fscanf(fd, "%f", &_tileSize);	   /// Read tile size (not used).

   assert(_xTiles == TILES_ON_X);
   assert(_yTiles == TILES_ON_Y);
   assert(nTiles <= TILES_ON_X * TILES_ON_Y);

	for(i = 0; i < nTiles; i++) {
		fscanf(fd, "%d", &_x);			/// Read x coords
		fscanf(fd, "%d", &_y);			/// Read y coords
		c = fscanf(fd, "%d", &tmp);	/// Read tile type
		assert(_x < TILES_ON_X);
		assert(_y < TILES_ON_Y);
		assert(tmp <= MAX_TILE_TYPES);

		tiles[_x][_y].setType(tmp);
		tiles[_x][_y].setCoordX(_x);
		tiles[_x][_y].setCoordY(_y);

   	/// There's a parameter. Read it.
		if(!c) {
		   /// fgets doesn't stop at white spaces but it stops at '\n'
			fgets(buf, MAX_PARAMETER_LENGTH, fd);
			tiles[_x][_y].setParameter(buf);
		   /// One loop is lost everytime a parameter is read.
			--i;
		}
	}

	fclose(fd);
	return true;
}
