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

int nConvexRooms;

C_BspTree::C_BspTree(USHORT depth)
{
   PRINT_FUNC_ENTRY;

	nBrushes = 0;
	nPolys = 0;
	scaleFactor = 1.0f;
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

	memset((void *)&treeStats, 0, sizeof(treeStats));
	memset((void *)&statistics, 0, sizeof(statistics));
}

C_BspTree::~C_BspTree(void)
{
   PRINT_FUNC_ENTRY;

	/// Delete data
	for(int i = 0 ; i < nBrushes ; i++) {
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			delete[] pBrushes[i].pPolys[j].pVertices;
			delete[] pBrushes[i].pPolys[j].pNorms;
		}
		delete[] pBrushes[i].pPolys;
	}

   delete[] pBrushes;
   delete[] pRawPolys;

	for(unsigned int i = 0; i < staticObjects.size(); ++i) {
	   delete staticObjects[i];
	}
	staticObjects.clear();

	delete headNode;
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
   float maxX = SMALLEST_FLOAT;
   float maxY = SMALLEST_FLOAT;
   float maxZ = SMALLEST_FLOAT;

   float minX = GREATEST_FLOAT;
   float minY = GREATEST_FLOAT;
   float minZ = GREATEST_FLOAT;

   printf("\n**********\n");
	printf("%s: Reading geometry file \"%s\"\n", __FUNCTION__, fileName);

	ifstream file(fileName , ios::in | ios::binary);

	if(!file.is_open()) {
	   printf("Couldn't open \"%s\"\n", fileName);
	   assert(0);
		return false;
	}

	file.read((char*)&nPolys , sizeof(int));
	printf("Total number of polygons: %d\n", nPolys);
	pRawPolys = new poly_t*[nPolys];
	int currentPoly = 0;

	/// Read number of brushes/meshes
	file.read((char*)&nBrushes , sizeof(int));
	printf("Number of brushes: %d\n", nBrushes);
	printf("Reading brushes... \n");

	pBrushes = new brush_t[nBrushes];

	/// For each brush...
	for(int i = 0 ; i < nBrushes ; i++) {
		/// ...read number of polys
		file.read((char*)&pBrushes[i].nPolys, sizeof(int));
		printf("   brush %d: %d polys\n", i, pBrushes[i].nPolys);

		pBrushes[i].pPolys = new poly_t[pBrushes[i].nPolys];

		/// For each poly in brush
		for(int j = 0 ; j < pBrushes[i].nPolys ; j++) {
			/// Read number of vertices
			file.read((char*)&pBrushes[i].pPolys[j].nVertices, sizeof(int));

			pBrushes[i].pPolys[j].pVertices = new C_Vertex[pBrushes[i].pPolys[j].nVertices];
			pBrushes[i].pPolys[j].pNorms = new C_Vertex[pBrushes[i].pPolys[j].nVertices];
			pBrushes[i].pPolys[j].usedAsDivider = false;

			pRawPolys[currentPoly] = &pBrushes[i].pPolys[j];
			currentPoly++;

			/// Read vertices
			for(int k = 0 ; k < pBrushes[i].pPolys[j].nVertices ; k++) {
				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].x , sizeof(float));
				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].y , sizeof(float));
				file.read((char*)&pBrushes[i].pPolys[j].pVertices[k].z , sizeof(float));

				pBrushes[i].pPolys[j].pVertices[k].x /= scaleFactor;
				pBrushes[i].pPolys[j].pVertices[k].y /= scaleFactor;
				pBrushes[i].pPolys[j].pVertices[k].z /= scaleFactor;

				/// Calculate tree's bbox
				if(pBrushes[i].pPolys[j].pVertices[k].x > maxX) maxX = pBrushes[i].pPolys[j].pVertices[k].x;
				if(pBrushes[i].pPolys[j].pVertices[k].y > maxY) maxY = pBrushes[i].pPolys[j].pVertices[k].y;
				if(pBrushes[i].pPolys[j].pVertices[k].z > maxZ) maxZ = pBrushes[i].pPolys[j].pVertices[k].z;

				if(pBrushes[i].pPolys[j].pVertices[k].x < minX) minX = pBrushes[i].pPolys[j].pVertices[k].x;
				if(pBrushes[i].pPolys[j].pVertices[k].y < minY) minY = pBrushes[i].pPolys[j].pVertices[k].y;
				if(pBrushes[i].pPolys[j].pVertices[k].z < minZ) minZ = pBrushes[i].pPolys[j].pVertices[k].z;
			}
		}
	}
	file.close();

	assert(currentPoly == nPolys);

   /// Set bbox
   bbox.SetMax(maxX , maxY , maxZ);
   bbox.SetMin(minX , minY , minZ);
   bbox.SetVertices();

	CalcNorms();

   printf("**********\n");

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

/**
 * Insert a mesh into the tree as a static object.
 * To determine to which node(s) the mesh belongs, all 8 bbox's vertices are
 * "thrown" down the tree
 */
void
C_BspTree::insertStaticObject(C_MeshGroup *staticMesh, ESMatrix *matrix)
{
   static unsigned int meshID = 0;
   C_Vertex bboxVertices[8];

   staticTreeObject_t *object = new staticTreeObject_t;
   object->mesh.softCopy(staticMesh);
   object->mesh.matrix = *matrix;
   object->meshID = meshID++;
   object->drawn = false;

   object->mesh.bbox.ApplyTransformation(matrix);
   object->mesh.bbox.GetVertices(bboxVertices);

   for(int i = 0; i < 8; ++i) {
      headNode->insertStaticObject(object, &bboxVertices[i]);
   }

   staticObjects.push_back(object);
}

void
C_BspTree::DistributeSamplePoints(void)
{
   for(unsigned int i = 0; i < leaves.size(); ++i) {
      leaves[i]->DistributePointsAlongBBox();
      leaves[i]->CleanUpPointSet(leaves[i]->pointSet, true, true);
   }
}

void
C_BspTree::BuildPVS(const char *filename)
{
	printf("%s\n", __FUNCTION__);

	/// An iparhei arheio me tin pliroforia diabase apo ekei
	bool pvsFileFound = false;
	pvsFileFound = this->ReadPVSFile(filename);

	cout << "Building PVS..." << endl;
	cout << "\tDistributing sample points... " << flush;
	DistributeSamplePoints();
	cout << "Done!" << endl;

	if(pvsFileFound) {
		cout << "\tTracing visibility... Found in file. Skipping calculations." << endl;
	} else {
		C_BspTree::TraceVisibility();
		cout << "Done!" << endl << endl;
	}

	cout << "Done!" << endl << endl;

	/// Write PVS into a file
	if(!pvsFileFound) {
		WritePVSFile(filename);
	}
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
   int tid = data->tid;
   C_BspTree *tree = data->tree;
   vector<C_BspNode *> &leaves = tree->leaves;
   C_BspNode *leaf1, *leaf2, *leaf3;
   const int progressBarStars = 20;

   C_BspNode *res;
   int load = leaves.size();
   int start = load / MAX_THREADS * tid;
   int end = load / MAX_THREADS * (tid + 1);
   float step = (float)progressBarStars / (float)load;

   /// thread 0 prints progress bar
   if(!tid) {
      pthread_mutex_lock(&mutex);
      progress = step;
      pthread_mutex_unlock(&mutex);

      cout << "\n\t\t0%|---------50---------|100%\n\t   ";
   }

	for(int l1 = start; l1 < end; l1++) {
	   leaf1 = leaves[l1];
		for(unsigned int l2 = 0; l2 < leaf1->PVS.size(); l2++) {
		   leaf2 = leaf1->PVS[l2];
         for(unsigned int l3 = 0; l3 < leaf2->PVS.size(); l3++) {
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
            if(!res)
               leaf1->addNodeToPVS(leaf3);

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
         for(int k = 0 ; k < (int)floor(progress + 0.5f); k++)
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


/// Find connected leaves
/// NOTE: Two leaves sharing at least one visibility point will be considered as connected
/// ----------------------
   printf("\tDetecting connected leaves... ");
   fflush(stdout);
   gettimeofday(&start, NULL);

	for(i = 0; i < nLeaves; i++) {
		for(j = 0; j < nLeaves; j++) {
			if(i == j || leaves[j]->visibleFrom[leaves[i]->nodeID] || leaves[i]->visibleFrom[leaves[j]->nodeID])
				continue;

         /// Loop through point sets
			for(unsigned int p1 = 0; p1 < leaves[i]->pointSet.size(); p1++) {
				for(unsigned int p2 = 0; p2 < leaves[j]->pointSet.size(); p2++) {
				   if(math::Distance(&leaves[i]->pointSet[p1], &leaves[j]->pointSet[p2]) < 5.0f) {
						if(leaves[j]->visibleFrom[leaves[i]->nodeID] == false) {
							leaves[j]->connectedLeaves.push_back(leaves[i]);
							leaves[j]->addNodeToPVS(leaves[i]);
						}

						if(leaves[i]->visibleFrom[leaves[j]->nodeID] == false) {
							leaves[i]->connectedLeaves.push_back(leaves[j]);
							leaves[i]->addNodeToPVS(leaves[j]);
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
   printf("Done! (%.2f s)\n", elapsedTime / 1000.0);

//return;

   /// After finding connected leaves try to furter clean up the visibility points
   /// Removes sample points that coincide with connected leaves' geometry
   printf("\tRemoving redundant sample points...");
   fflush(stdout);

   C_BspNode *leaf;
   for(i = 0; i < nLeaves; i++) {
      leaf = leaves[i];
//      C_BspNode::CleanUpPointSet(leaf, leaf->pointSet, false, true);
      for(j = 0; j < (int)leaf->PVS.size(); j++) {
         leaf->PVS[j]->CleanUpPointSet(leaf->pointSet, false, true);
      }
   }
   printf("Done!\n");

//   return;

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

   printf("\n\nDone (%.2f s)\n", elapsedTime / 1000.0f);

	pthread_mutex_destroy(&mutex);
}

C_BspNode *
C_BspTree::CheckVisibility(C_BspNode *node1, C_BspNode *node2)
{
   C_BspNode *occluderNode = NULL;

	for(unsigned int p1 = 0; p1 < node1->pointSet.size(); p1++) {
		for(unsigned int p2 = 0; p2 < node2->pointSet.size(); p2++) {
			if((occluderNode = C_BspTree::RayIntersectsSomethingInTree(headNode, &node1->pointSet[p1], &node2->pointSet[p2])) == NULL) {
				return NULL;
			}
		}
	}

	return occluderNode;
}

C_BspNode *
C_BspTree::RayIntersectsSomethingInTree(C_BspNode *node, C_Vertex *start, C_Vertex *end)
{
   if(!node)
      return NULL;

   C_BspNode *occluderNode;

	if(node->isLeaf) {
		for(int cp = 0 ; cp < node->nTriangles ; cp++) {
			if(RayTriangleIntersection(start, end, &node->triangles[cp])) {
				return node;
			}
		}
		return NULL;
	}

	int startSide = ClassifyVertex(&node->partitionPlane, start);
	int endSide = ClassifyVertex(&node->partitionPlane, end);

   /// If the ray spans the node's partition plane, then send the ray down both sides of node
	if((startSide == COINCIDENT && endSide == COINCIDENT) ||
      (startSide != endSide && startSide != COINCIDENT && endSide != COINCIDENT)) {
		if((occluderNode = C_BspTree::RayIntersectsSomethingInTree(node->backNode, start, end))) {
			return occluderNode;
		}
		if((occluderNode = C_BspTree::RayIntersectsSomethingInTree(node->frontNode, start, end))) {
			return occluderNode;
		}
	}

   /// If ray is whole in front of partition plane the send it down the front node
   /// The or in the if statement is because one of the points might be coinciding with the plane.
	if(startSide == FRONT || endSide == FRONT) {
		if((occluderNode = C_BspTree::RayIntersectsSomethingInTree(node->frontNode, start, end))) {
			return occluderNode;
		}
	}

   /// Respectively send it down to back node
   /// The or in the if statement is because one of the points might be coinciding with the plane.
	if(startSide == BACK || endSide == BACK) {
		if((occluderNode = C_BspTree::RayIntersectsSomethingInTree(node->backNode, start, end))) {
			return occluderNode;
		}
	}

	return NULL;
}

void
C_BspTree::BuildBspTree(void)
{
	cout << "***********************************************" << endl;
	cout << "Building bsp tree... ";

	headNode = new C_BspNode(pRawPolys , nPolys);
	headNode->BuildBspTree(this);

   /// Close possible space holes between the tree's leaves
   printf("\n\tDetecting and closing space holes... ");
   fflush(stdout);

   closeLeafHoles();

   printf("Done!\n");

	TessellatePolygons();

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
C_BspTree::Draw2(C_Camera *camera)
{
	/// Set all leaves as not drawn
	for(unsigned int i = 0 ; i < leaves.size() ; i++) {
		leaves[i]->drawn = false;
	}

   /// Pass matrices to shader
	/// Keep a copy of global movelview matrix
	shaderManager->pushShader(bspShader);
      glEnableVertexAttribArray(bspShader->verticesAttribLocation);
      glEnableVertexAttribArray(bspShader->normalsAttribLocation);

      ESMatrix mat = globalViewMatrix;
      esTranslate(&mat, position.x , position.y , position.z);

      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODELVIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);
      headNode->Draw(camera, this, false);
	shaderManager->popShader();

	return 0;
}

int
C_BspTree::Draw_PVS(C_Camera *camera)
{
   /// Initialize tree statistics
	memset((void *)&statistics, 0, sizeof(statistics));

   /// Set all leaves as not drawn
	for(unsigned int i = 0 ; i < leaves.size() ; i++) {
		leaves[i]->drawn = false;
		for(unsigned int j = 0; j < leaves[i]->staticObjects.size(); ++j)
		   leaves[i]->staticObjects[j]->drawn = false;
	}

   /// Pass matrices to shader
	/// Keep a copy of global movelview matrix
	if(DRAW_BSP_GEOMETRY) {
   	shaderManager->pushShader(bspShader);
      ESMatrix mat = Identity;
      esTranslate(&mat, position.x , position.y , position.z);

      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_VIEW_MATRIX, 1, GL_FALSE, (GLfloat *)&globalViewMatrix.m[0][0]);
      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_MODEL_MATRIX, 1, GL_FALSE, (GLfloat *)&mat.m[0][0]);
      bspShader->setUniformMatrix4fv(UNIFORM_VARIABLE_NAME_PROJECTION_MATRIX, 1, GL_FALSE, (GLfloat *)&globalProjectionMatrix.m[0][0]);
   }

   headNode->Draw(camera, this, USE_PVS);

   if(DRAW_BSP_GEOMETRY) {
      shaderManager->popShader();
   }

	return 0;
}

void
C_BspTree::TessellatePolygons(void)
{
	headNode->TessellatePolygonsInLeaves();
}

void
C_BspTree::WritePVSFile(const char *fileName)
{
	fstream filestr;
	filestr.open(fileName, fstream::out);

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

void
C_BspTree::dumpSamplePoints(const char *filename)
{
   unsigned int i, j, sp;
   C_Vertex p;
   C_BspNode leaf;
   FILE *fp;

   fp = fopen(filename, "w");
   assert(fp);

   for(i = 0; i < leaves.size(); i++) {
      fprintf(fp, "Node: %lu (connected to: ", leaves[i]->nodeID);

      for(j = 0; j < leaves[i]->connectedLeaves.size(); j++)
         fprintf(fp, "%lu ", leaves[i]->connectedLeaves[j]->nodeID);

      fprintf(fp, ")\n");

      for(sp = 0; sp < leaves[i]->pointSet.size(); sp++) {
         p = leaves[i]->pointSet[sp];
         fprintf(fp, "   (%.3f, %.3f, %.3f)\n", p.x, p.y, p.z);
      }
   }

   fclose(fp);
}

/**
 * Scan all the leaves and find gaps between them. Gaps possibly mean glitches in the tree's PVS.
 * Steps are:
 * 1. For every leaf in the tree loop through all the other leaves and see if all its
 *    4 (6) faces, -X, +X, -Z, +Z (-Y, +Y) are coincident with an other leaf.
 * 2. If it is found mark that face as adjacent to that node.
 * 3. Else try to find the best leaf candidate to connect the leaf to.
 */
void
C_BspTree::closeLeafHoles(void)
{
   int i;
   float dist;
   C_BspNode *leaf1, *leaf2;
   C_Vertex treeMin, treeMax, leaf1Min, leaf1Max, leaf2Min, leaf2Max;

   typedef struct {
      vector<C_BspNode *> adjacentNodes[TOTAL_FACES];
      vector<C_BspNode *> bestNodeToConnect[TOTAL_FACES];
      float bestNodeDistances[TOTAL_FACES];
      bool boundaryLeaf;
   } nodeAdjacentData_t;
   nodeAdjacentData_t *aData = new nodeAdjacentData_t[leaves.size()];

   /// Initialize variables
   /// ----------------------
	for(int i = 0 ; i < nLeaves; i++) {
		leaves[i]->visibleFrom = new bool[nNodes];
		leaves[i]->checkedVisibilityWith = new bool[nNodes];

		memset(leaves[i]->visibleFrom, false, nNodes * sizeof(bool));
		memset(leaves[i]->checkedVisibilityWith, false, nNodes * sizeof(bool));
	}

   bbox.GetMin(&treeMin);
   bbox.GetMax(&treeMax);

   for(unsigned int b1 = 0; b1 < leaves.size(); b1++) {
      nodeAdjacentData_t *cNode = &aData[b1];
      leaf1 = leaves[b1];
      leaf1->bbox.GetMin(&leaf1Min);
      leaf1->bbox.GetMax(&leaf1Max);

      /// Init node's data
      cNode->boundaryLeaf = false;
      for(i = 0; i < TOTAL_FACES; i++) {
         cNode->bestNodeDistances[i] = GREATEST_FLOAT;
      }

      /// Take into consideration that a leaf might be at the boundary of the tree
      if(FLOAT_EQ(leaf1Min.x, treeMin.x)) { cNode->adjacentNodes[X_MINUS].push_back(NULL); cNode->boundaryLeaf = true; }
      if(FLOAT_EQ(leaf1Max.x, treeMax.x)) { cNode->adjacentNodes[X_PLUS].push_back(NULL);  cNode->boundaryLeaf = true; }
      if(FLOAT_EQ(leaf1Min.z, treeMin.z)) { cNode->adjacentNodes[Z_MINUS].push_back(NULL); cNode->boundaryLeaf = true; }
      if(FLOAT_EQ(leaf1Max.z, treeMax.z)) { cNode->adjacentNodes[Z_PLUS].push_back(NULL);  cNode->boundaryLeaf = true; }
      if(FLOAT_EQ(leaf1Min.y, treeMin.y)) { cNode->adjacentNodes[Y_MINUS].push_back(NULL); cNode->boundaryLeaf = true; } else assert(0);
      if(FLOAT_EQ(leaf1Max.y, treeMax.y)) { cNode->adjacentNodes[Y_PLUS].push_back(NULL);  cNode->boundaryLeaf = true; } else assert(0);

      for(unsigned int b2 = 0; b2 < leaves.size(); b2++) {
         if(b1 == b2)
            continue;

         leaf2 = leaves[b2];

         leaf2->bbox.GetMin(&leaf2Min);
         leaf2->bbox.GetMax(&leaf2Max);

         /// X_MINUS
         if(((FLOAT_GREATER(leaf1Min.z, leaf2Min.z) && FLOAT_SMALLER(leaf1Min.z, leaf2Max.z)) ||
         (FLOAT_GREATER(leaf2Min.z, leaf1Min.z) && FLOAT_SMALLER(leaf2Min.z, leaf1Max.z)) ||

         (FLOAT_EQ(leaf2Min.z, leaf1Min.z) && FLOAT_SMALLER(leaf1Max.z, leaf2Max.z)) ||
         (FLOAT_EQ(leaf2Min.z, leaf1Min.z) && FLOAT_GREATER(leaf1Max.z, leaf2Max.z)) ||

         (FLOAT_EQ(leaf2Max.z, leaf1Max.z) && FLOAT_SMALLER(leaf1Min.z, leaf2Min.z)) ||
         (FLOAT_EQ(leaf2Max.z, leaf1Max.z) && FLOAT_GREATER(leaf1Min.z, leaf2Min.z)) ||

         (FLOAT_EQ(leaf1Min.z, leaf2Min.z) && FLOAT_EQ(leaf1Max.z, leaf2Max.z)))) {
            if(FLOAT_EQ(leaf1Min.x, leaf2Max.x)) {
               /// Adjacent leaf found
               cNode->adjacentNodes[X_MINUS].push_back(leaf2);
            } else if(FLOAT_GREATER(leaf1Min.x, leaf2Max.x)) {
               /// Else calculate distance
               dist = leaf1Min.x - leaf2Max.x;
//               if(dist < cNode->bestNodeDistances[X_MINUS]) {
               if(FLOAT_SMALLER(dist, cNode->bestNodeDistances[X_MINUS])) {
                  cNode->bestNodeDistances[X_MINUS] = dist;
//                  cNode->bestNodeToConnect[X_MINUS] = leaf2;
                  cNode->bestNodeToConnect[X_MINUS].clear();
                  cNode->bestNodeToConnect[X_MINUS].push_back(leaf2);
               } else if(FLOAT_EQ(dist, cNode->bestNodeDistances[X_MINUS])) {
                  cNode->bestNodeToConnect[X_MINUS].push_back(leaf2);
               }
            }
         }

         /// X_PLUS
         if(((FLOAT_GREATER(leaf1Min.z, leaf2Min.z) && FLOAT_SMALLER(leaf1Min.z, leaf2Max.z)) ||
         (FLOAT_GREATER(leaf2Min.z, leaf1Min.z) && FLOAT_SMALLER(leaf2Min.z, leaf1Max.z)) ||

         (FLOAT_EQ(leaf2Min.z, leaf1Min.z) && FLOAT_SMALLER(leaf1Max.z, leaf2Max.z)) ||
         (FLOAT_EQ(leaf2Min.z, leaf1Min.z) && FLOAT_GREATER(leaf1Max.z, leaf2Max.z)) ||

         (FLOAT_EQ(leaf2Max.z, leaf1Max.z) && FLOAT_SMALLER(leaf1Min.z, leaf2Min.z)) ||
         (FLOAT_EQ(leaf2Max.z, leaf1Max.z) && FLOAT_GREATER(leaf1Min.z, leaf2Min.z)) ||

         (FLOAT_EQ(leaf1Min.z, leaf2Min.z) && FLOAT_EQ(leaf1Max.z, leaf2Max.z)))) {
            if(FLOAT_EQ(leaf1Max.x, leaf2Min.x)) {
               /// Adjacent leaf found
               cNode->adjacentNodes[X_PLUS].push_back(leaf2);
            } else if(FLOAT_SMALLER(leaf1Max.x, leaf2Min.x)) {
               /// Else calculate distance
               dist = leaf2Min.x - leaf1Max.x;
               if(FLOAT_SMALLER(dist, cNode->bestNodeDistances[X_PLUS])) {
                  cNode->bestNodeDistances[X_PLUS] = dist;
//                  cNode->bestNodeToConnect[X_PLUS] = leaf2;
                  cNode->bestNodeToConnect[X_PLUS].clear();
                  cNode->bestNodeToConnect[X_PLUS].push_back(leaf2);
               } else if(FLOAT_EQ(dist, cNode->bestNodeDistances[X_PLUS])) {
                  cNode->bestNodeToConnect[X_PLUS].push_back(leaf2);
               }
            }
         }

         /// Z_MINUS
         if(((FLOAT_GREATER(leaf1Min.x, leaf2Min.x) && FLOAT_SMALLER(leaf1Min.x, leaf2Max.x)) ||
         (FLOAT_GREATER(leaf2Min.x, leaf1Min.x) && FLOAT_SMALLER(leaf2Min.x, leaf1Max.x)) ||

         (FLOAT_EQ(leaf2Min.x, leaf1Min.x) && FLOAT_SMALLER(leaf1Max.x, leaf2Max.x)) ||
         (FLOAT_EQ(leaf2Min.x, leaf1Min.x) && FLOAT_GREATER(leaf1Max.x, leaf2Max.x)) ||

         (FLOAT_EQ(leaf2Max.x, leaf1Max.x) && FLOAT_SMALLER(leaf1Min.x, leaf2Min.x)) ||
         (FLOAT_EQ(leaf2Max.x, leaf1Max.x) && FLOAT_GREATER(leaf1Min.x, leaf2Min.x)) ||

         (FLOAT_EQ(leaf1Min.x, leaf2Min.x) && FLOAT_EQ(leaf1Max.x, leaf2Max.x)))) {
            if(FLOAT_EQ(leaf1Min.z, leaf2Max.z)) {
               /// Adjacent leaf found
               cNode->adjacentNodes[Z_MINUS].push_back(leaf2);
            } else if(FLOAT_GREATER(leaf1Min.z, leaf2Max.z)) {
               /// Else calculate distance
               dist = leaf1Min.z - leaf2Max.z;
               if(FLOAT_SMALLER(dist, cNode->bestNodeDistances[Z_MINUS])) {
                  cNode->bestNodeDistances[Z_MINUS] = dist;
//                  cNode->bestNodeToConnect[Z_MINUS] = leaf2;
                  cNode->bestNodeToConnect[Z_MINUS].clear();
                  cNode->bestNodeToConnect[Z_MINUS].push_back(leaf2);
               } else if(FLOAT_EQ(dist, cNode->bestNodeDistances[Z_MINUS])) {
                  cNode->bestNodeToConnect[Z_MINUS].push_back(leaf2);
               }
            }
         }

         /// Z_PLUS
         if(((FLOAT_GREATER(leaf1Min.x, leaf2Min.x) && FLOAT_SMALLER(leaf1Min.x, leaf2Max.x)) ||
         (FLOAT_GREATER(leaf2Min.x, leaf1Min.x) && FLOAT_SMALLER(leaf2Min.x, leaf1Max.x)) ||

         (FLOAT_EQ(leaf2Min.x, leaf1Min.x) && FLOAT_SMALLER(leaf1Max.x, leaf2Max.x)) ||
         (FLOAT_EQ(leaf2Min.x, leaf1Min.x) && FLOAT_GREATER(leaf1Max.x, leaf2Max.x)) ||

         (FLOAT_EQ(leaf2Max.x, leaf1Max.x) && FLOAT_SMALLER(leaf1Min.x, leaf2Min.x)) ||
         (FLOAT_EQ(leaf2Max.x, leaf1Max.x) && FLOAT_GREATER(leaf1Min.x, leaf2Min.x)) ||

         (FLOAT_EQ(leaf1Min.x, leaf2Min.x) && FLOAT_EQ(leaf1Max.x, leaf2Max.x)))) {
            if(FLOAT_EQ(leaf1Max.z, leaf2Min.z)) {
               /// Adjacent leaf found
               cNode->adjacentNodes[Z_PLUS].push_back(leaf2);
            } else if(FLOAT_SMALLER(leaf1Max.z, leaf2Min.z)) {
               /// Else calculate distance
               dist = leaf2Min.z - leaf1Max.z;
               if(FLOAT_SMALLER(dist, cNode->bestNodeDistances[Z_PLUS])) {
                  cNode->bestNodeDistances[Z_PLUS] = dist;
//                  cNode->bestNodeToConnect[Z_PLUS] = leaf2;
                  cNode->bestNodeToConnect[Z_PLUS].clear();
                  cNode->bestNodeToConnect[Z_PLUS].push_back(leaf2);
               } else if(FLOAT_EQ(dist, cNode->bestNodeDistances[Z_PLUS])) {
                  cNode->bestNodeToConnect[Z_PLUS].push_back(leaf2);
               }
            }
         }
      }

      /// If for at least one node hasn't been found an adjacent node
      if(!cNode->adjacentNodes[X_MINUS].size() || !cNode->adjacentNodes[X_PLUS].size() ||
         !cNode->adjacentNodes[Z_MINUS].size() || !cNode->adjacentNodes[Z_PLUS].size() ||
         !cNode->adjacentNodes[Y_MINUS].size() || !cNode->adjacentNodes[Y_PLUS].size()) {

         printf("\nleaf %lu is not tightly packed. ", leaf1->nodeID);
         printf("Uncovered faces are: ");
         for(int i = 0; i < TOTAL_FACES; i++) {
            if(!cNode->adjacentNodes[i].size()) {
               printf("\n\t\t\t%s. ", C_BBox::ADJACENT_FACE_NAMES[i]);
               printf("Best candidates: ");

               if(cNode->bestNodeToConnect[i].size()) {
//                  printf("+_+_+_ %d\n", cNode->bestNodeToConnect[i].size());
                  for(unsigned int jj = 0; jj < cNode->bestNodeToConnect[i].size(); ++jj)
                    printf("%lu ", cNode->bestNodeToConnect[i][jj]->nodeID);
               } else {
                  if(cNode->boundaryLeaf) {
                     printf("none! Boundary leaf.");
                  } else {
                     printf("none and not a boundary leaf.");
                     assert(0);
                  }
               }
            }
         }

         /// Connect leaf1 with all candidate nodes
         C_Vertex min, max;
         for(int i = 0; i < TOTAL_FACES; i++) {
            if(!cNode->adjacentNodes[i].size() && cNode->bestNodeToConnect[i].size()) {
               leaf1->bbox.GetMax(&max);
               leaf1->bbox.GetMin(&min);
               for(unsigned int jj = 0; jj < cNode->bestNodeToConnect[i].size(); ++jj) {
//                     leaf1->addNodeToPVS(cNode->bestNodeToConnect[i][jj]);

                  /// Enlarge bounding boxes
                  switch(i) {
                  case X_MINUS:
                     leaf1->bbox.SetMin(min.x - cNode->bestNodeDistances[X_MINUS], min.y, min.z);
                     leaf1->bbox.SetVertices();
                     break;
                  case X_PLUS:
                     leaf1->bbox.SetMax(max.x + cNode->bestNodeDistances[X_PLUS], max.y, max.z);
                     leaf1->bbox.SetVertices();
                     break;
                  case Z_PLUS:
                     leaf1->bbox.SetMax(max.x, max.y, max.z + cNode->bestNodeDistances[Z_PLUS]);
                     leaf1->bbox.SetVertices();
                     break;
                  case Z_MINUS:
                     leaf1->bbox.SetMin(min.x, min.y, min.z - cNode->bestNodeDistances[Z_MINUS]);
                     leaf1->bbox.SetVertices();
                     break;
                  } // switch
               } // for(int jj
            } // if
         } // for
     }
   }

   delete[] aData;
}
