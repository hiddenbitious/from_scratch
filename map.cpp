#include <stdio.h>
#include <stdlib.h>
#include "map.h"

C_Map::C_Map(void)
{
   PRINT_FUNC_ENTRY;

   bspTree = NULL;

   for(int x = 0; x < TILES_ON_X; x++) {
		for(int y = 0; y < TILES_ON_Y; y++) {
		   tiles[x][y].setCoordX(x);
		   tiles[x][y].setCoordY(y);
		   tiles[x][y].setArea(AREA_NAN);
		   tiles[x][y].hasParameter = false;
		}
	}
}

C_Vertex
C_Map::cameraStartPosition(void)
{
   C_Vertex cameraPosition;
   bool found = false;
   int x, y;

   for(x = 0; x < TILES_ON_X && !found; x++) {
		for(y = 0; y < TILES_ON_Y && !found; y++) {
		   if(tiles[x][y].getType() == TILE_4)
		      found = true;
		}
   }

   cameraPosition.x = y * TILE_SIZE - TILE_SIZE / 2.0f;
   cameraPosition.y = TILE_SIZE / 2.0f;
   cameraPosition.z = x * TILE_SIZE - TILE_SIZE / 2.0f;

   return cameraPosition;
}

C_Map::~C_Map()
{
   PRINT_FUNC_ENTRY;

   if(bspTree) {
      delete bspTree;
      bspTree = NULL;
   }

   for(int x = 0; x < TILES_ON_X; x++) {
		for(int y = 0; y < TILES_ON_Y; y++) {
         tiles[x][y].~tile();
      }
   }
}

bool
C_Map::createMap(const char *filename)
{
   bool ret;

   printf("Creating map...\n");

   /// Read the tile data
   ret = readMap(filename);
   if(!ret) {
      assert(0);
      return false;
   }

   bspTree = new C_BspTree(6);

   /// Read bsp geometry and build the bsp tree
   bspTree->ReadGeometryFile("mapGeometry.bsp");
   bspTree->BuildBspTree();
   bspTree->BuildPVS();

   /// Load 3d meshes
   load3DObjects();

   /// Position the walls
   placeObjects();

   return true;
}

/**
 * Position the walls into the bsp tree as static meshes
 */
bool
C_Map::placeObjects(void)
{
   static float scale = 1.07f;

   ESMatrix mat;
   for(int x = 0; x < TILES_ON_X; x++) {
      for(int y = 0; y < TILES_ON_Y; y++) {

         if(tiles[x][y].getType() == TILE_WALL) {
            /// Left wall
            if(x > 0 && tiles[x-1][y].getArea() == AREA_WALKABLE) {
               esMatrixLoadIdentity(&mat);

               esTranslate(&mat, (float)(y + 1) * TILE_SIZE, 0.0f, (float)x * TILE_SIZE);
               esScale(&mat, scale, scale, scale);
               esRotate(&mat, 180.0f, 0.0f, 1.0f, 0.0f);
               bspTree->insertStaticObject(&wallMesh, &mat);
            }

            /// Upper wall
            if(y < TILES_ON_Y - 1 && tiles[x][y+1].getArea() == AREA_WALKABLE) {
               esMatrixLoadIdentity(&mat);

               esTranslate(&mat, (float)(y+1) * TILE_SIZE, 0.0f, (float)(x+1) * TILE_SIZE);
               esScale(&mat, scale, scale, scale);
               esRotate(&mat, 270.0f, 0.0f, 1.0f, 0.0f);
               bspTree->insertStaticObject(&wallMesh, &mat);
            }

            /// Right wall
            if(x < TILES_ON_X - 1 && tiles[x+1][y].getArea() == AREA_WALKABLE) {
               esMatrixLoadIdentity(&mat);

               esTranslate(&mat, (float)y * TILE_SIZE, 0.0f, (float)(x+1) * TILE_SIZE);
               esScale(&mat, scale, scale, scale);
               bspTree->insertStaticObject(&wallMesh, &mat);
            }

            /// Bottom wall
            if(y > 0 && tiles[x][y-1].getArea() == AREA_WALKABLE) {
               esMatrixLoadIdentity(&mat);

               esTranslate(&mat, (float)y * TILE_SIZE, 0.0f, (float)x * TILE_SIZE);
               esScale(&mat, scale, scale, scale);
               esRotate(&mat, 90.0f, 0.0f, 1.0f, 0.0f);
               bspTree->insertStaticObject(&wallMesh, &mat);
            }
         } else if(tiles[x][y].getArea() == AREA_WALKABLE) {
            esMatrixLoadIdentity(&mat);

            esTranslate(&mat, (float)y * TILE_SIZE, 0.0f, (float)x * TILE_SIZE);
            bspTree->insertStaticObject(&floorMesh, &mat);
         }
      }
   }

   return true;
}

bool
C_Map::load3DObjects(void)
{
   /// This is supposed to check every tile in the map
   /// and load the needed 3d mesh from disk

   /// Obiously an mesh manager will be needed to avoid loading
   /// the same mesh more than once

//   wallMesh.loadFromFile("objmodels/cube.obj");
   wallMesh.loadFromFile("wallMeshes/wall_01.obj");
   wallMesh.shader = wallShader;

   floorMesh.loadFromFile("wallMeshes/floor_01.obj");
   floorMesh.shader = wallShader;

   /// Scale wall mesh
   C_Vertex min, max;
   wallMesh.bbox.GetMax(&max);
   wallMesh.bbox.GetMin(&min);
   float xLen = max.x - min.x;
   float zLen = max.z - min.z;
   float scale = TILE_SIZE / MAX(xLen, zLen);
   /// Scale it to fit TILE_SIZE
   ESMatrix matrix = Identity;
   esScale(&matrix, scale, scale, scale);

   if(zLen > xLen) {
      esTranslate(&matrix, 0.0f, 0.0f, xLen / 2.0f );
      /// Orient it to face down the +z
      esRotate(&matrix, 270.0f, 0.0f, 1.0f, 0.0f);
   } else {
      esTranslate(&matrix, zLen / 2.0f, 0.0f, 0.0f );
   }

   wallMesh.applyTransformationOnVertices(&matrix);

   /// Scale floor mesh
   floorMesh.bbox.GetMax(&max);
   floorMesh.bbox.GetMin(&min);
   xLen = max.x - min.x;
   zLen = max.z - min.z;
   scale = TILE_SIZE / MAX(xLen, zLen);
   matrix = Identity;
   /// Translate it a bit
   esTranslate(&matrix, TILE_SIZE / 2.0f, 0.0f, TILE_SIZE / 2.0f);
   /// Scale it to fit TILE_SIZE
   esScale(&matrix, scale, scale, scale);
   floorMesh.applyTransformationOnVertices(&matrix);

   return true;
}

void
C_Map::draw(C_Camera *camera)
{
   int mapPolys;

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//   floorMesh.draw(camera);
   mapPolys = bspTree->Draw_PVS(camera);


	int line = 2;
	int lineHeight = 18;

	camera->PrintText(0, lineHeight * line++,
					 1.0f, 1.0f, 0.0f, 0.6f,
					 "total leaves: %d. Drawn: %d" , bspTree->statistics.totalLeaves, bspTree->statistics.leavesDrawn);

	camera->PrintText(0, lineHeight * line++,
					 1.0f, 1.0f, 0.0f, 0.6f,
					 "total objects: %d. Drawn: %d" , bspTree->statistics.totalStaticObjects, bspTree->statistics.staticObjectsDrawn);

	camera->PrintText(0, lineHeight * line++,
					 1.0f, 1.0f, 0.0f, 0.6f,
					 "total triangles: %d. Drawn: %d" , bspTree->statistics.totalTriangles, bspTree->statistics.trianglesDrawn);

//   /// Print statistics
//   printf("Draw statistics:\n");
//   printf("\ttotal leaves:%d\n", bspTree->statistics.totalLeaves);
//   printf("\tleaves drawn:%d\n", bspTree->statistics.leavesDrawn);
//   printf("\ttotal triangles:%d\n", bspTree->statistics.totalTriangles);
//   printf("\ttriangles drawn:%d\n", bspTree->statistics.trianglesDrawn);
//   printf("\ttotal static objects:%d\n", bspTree->statistics.totalStaticObjects);
//   printf("\tstatic objects drawn:%d\n", bspTree->statistics.staticObjectsDrawn);
}

bool
C_Map::readMap(const char *filename)
{
   printf("*********\n");
   printf("Reading map file \"%s\"\n", filename);

	int _xTiles, _yTiles, type, area, nTiles, _x, _y;
	int i, c, sum = 0, nWalkable = 0, nVoid = 0;
	char buf[MAX_PARAMETER_LENGTH];
	int counters[N_TILE_TYPES] = {0};

	FILE *fd;

	if((fd = fopen(filename, "r")) == NULL)
		return false;

	c = fscanf(fd, "%d", &_xTiles);		/// Read size on x.
	c = fscanf(fd, "%d", &_yTiles);		/// Read size on y.
	c = fscanf(fd, "%d", &nTiles);		   /// Read number of tiles stored in file.

   assert(_xTiles == TILES_ON_X);
   assert(_yTiles == TILES_ON_Y);
   assert(nTiles == TILES_ON_X * TILES_ON_Y);

	for(i = 0; i < nTiles; i++) {
		c = fscanf(fd, "%d", &_x);			/// Read x coords
		c = fscanf(fd, "%d", &_y);			/// Read y coords
		c = fscanf(fd, "%d", &type);	   /// Read tile type
		c = fscanf(fd, "%d", &area);	/// Read tile area

		assert(_x < TILES_ON_X);
		assert(_y < TILES_ON_Y);
		assert(type < N_TILE_TYPES);
		assert(area < N_AREA_TYPES);

		tiles[_x][_y].setType((tileTypes_t)type);
		tiles[_x][_y].setArea((areaTypes_t)area);
		tiles[_x][_y].setCoordX(_x);
		tiles[_x][_y].setCoordY(_y);

		/// Count tiles
		++counters[type];
		++sum;
		if(area == AREA_VOID) {
		   ++nVoid;
      } else if(area == AREA_WALKABLE) {
         ++nWalkable;
      }

   	/// There's a parameter. Read it.
		if(!c) {
		   /// fgets doesn't stop at white spaces but it stops at '\n'
			fgets(buf, MAX_PARAMETER_LENGTH, fd);
//			tiles[_x][_y].setParameter(buf);

		   /// One loop is lost everytime a parameter is read.
         if(area == AREA_VOID) {
            --nVoid;
         } else if(area == AREA_WALKABLE) {
            --nWalkable;
         }

			--i;
			--sum;
         --counters[type];
		}
	}

	fclose(fd);

	printf("\tFound:\n");
	for(int i = 0; i < N_TILE_TYPES; i++) {
	   if(counters[i])
         printf("\tType %d: %d tiles\n", i, counters[i]);
	}
	printf("\t%d walkable and %d void tiles\n", nWalkable, nVoid);

	printf("\tTotal %d tiles\n", sum);
	printf("Done.\n");
   printf("*********\n");

	return true;
}
