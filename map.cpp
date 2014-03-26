#include <stdio.h>
#include <stdlib.h>
#include "map.h"

C_Map::C_Map(void)
{
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

C_Map::~C_Map()
{
   PRINT_FUNC_ENTRY;
   if(bspTree) {
      delete bspTree;
      bspTree = NULL;
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
   ESMatrix mat;
   for(int x = 0; x < TILES_ON_X; x++) {
      for(int y = 0; y < TILES_ON_Y; y++) {

         if(tiles[x][y].getType() != TILE_WALL)
            continue;

         /// Left wall
         if(x > 0 && tiles[x-1][y].getArea() == AREA_WALKABLE) {
            esMatrixLoadIdentity(&mat);

            esTranslate(&mat, (float)y * TILE_SIZE, 0.0f, (float)x * TILE_SIZE);
            bspTree->insertStaticObject(&wallMesh, &mat);
         }

         /// Upper wall
         if(y < TILES_ON_Y - 1 && tiles[x][y+1].getArea() == AREA_WALKABLE) {
            esMatrixLoadIdentity(&mat);

            esTranslate(&mat, (float)(y+1) * TILE_SIZE, 0.0f, (float)(x+1) * TILE_SIZE);
            esRotate(&mat, 270.0f, 0.0f, 1.0f, 0.0f);
            bspTree->insertStaticObject(&wallMesh, &mat);
         }

         /// Right wall
         if(x < TILES_ON_X - 1 && tiles[x+1][y].getArea() == AREA_WALKABLE) {
            esMatrixLoadIdentity(&mat);

            esTranslate(&mat, (float)y * TILE_SIZE, 0.0f, (float)(x+1) * TILE_SIZE);
            bspTree->insertStaticObject(&wallMesh, &mat);
         }

         /// Bottom wall
         if(y > 0 && tiles[x][y-1].getArea() == AREA_WALKABLE) {
            esMatrixLoadIdentity(&mat);

            esTranslate(&mat, (float)y * TILE_SIZE, 0.0f, (float)x * TILE_SIZE);
            esRotate(&mat, 90.0f, 0.0f, 1.0f, 0.0f);
            bspTree->insertStaticObject(&wallMesh, &mat);
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

   wallMesh.loadFromFile("objmodels/cube.obj");
//   wallMesh.loadFromFile("wallMeshes/wall_01.obj");
   wallMesh.shader = wallShader;

   C_Mesh *mesh = wallMesh.meshes;
//   int index;
//   for(int i = 0; i < mesh->nTriangles; ++i) {
//      index = mesh->indices[3 * i];
//      printf("%d: %f %f %f\n", index, mesh->vertices[index].x, mesh->vertices[index].y, mesh->vertices[index].z);
//      printf("%d: %f %f\n", index, mesh->textCoords[index].u, mesh->textCoords[index].v);
//
//      index = mesh->indices[3 * i + 1];
//      printf("%d: %f %f %f\n", index, mesh->vertices[index].x, mesh->vertices[index].y, mesh->vertices[index].z);
//      printf("%d: %f %f\n", index, mesh->textCoords[index].u, mesh->textCoords[index].v);
//
//      index = mesh->indices[3 * i + 2];
//      printf("%d: %f %f %f\n", index, mesh->vertices[index].x, mesh->vertices[index].y, mesh->vertices[index].z);
//      printf("%d: %f %f\n\n", index, mesh->textCoords[index].u, mesh->textCoords[index].v);
//   }


   while(mesh) {
      assert(!(mesh->nVertices%3));
      for(unsigned int i = 0; i < mesh->nVertices / 3; i++) {
         printf("v%d(%f %f %f) -- ", i, mesh->vertices[3 * i    ].x,
                                        mesh->vertices[3 * i    ].y,
                                        mesh->vertices[3 * i    ].z);

         printf("(%f %f %f) -- ", mesh->vertices[3 * i + 1].x,
                                  mesh->vertices[3 * i + 1].y,
                                  mesh->vertices[3 * i + 1].z);

         printf("(%f %f %f)\n",   mesh->vertices[3 * i + 2].x,
                                  mesh->vertices[3 * i + 2].y,
                                  mesh->vertices[3 * i + 2].z);


         printf("n%d(%f %f %f) -- ", i, mesh->normals[3 * i    ].x,
                                        mesh->normals[3 * i    ].y,
                                        mesh->normals[3 * i    ].z);

         printf("(%f %f %f) -- ", mesh->normals[3 * i + 1].x,
                                  mesh->normals[3 * i + 1].y,
                                  mesh->normals[3 * i + 1].z);

         printf("(%f %f %f)\n",   mesh->normals[3 * i + 2].x,
                                  mesh->normals[3 * i + 2].y,
                                  mesh->normals[3 * i + 2].z);

         if(mesh->textCoords) {
            printf("t%d(%f %f) -- ", i, mesh->textCoords[3 * i    ].u,
                                        mesh->textCoords[3 * i    ].v);

            printf("(%f %f) -- ", mesh->textCoords[3 * i + 1].u,
                                  mesh->textCoords[3 * i + 1].v);

            printf("(%f %f)\n\n", mesh->textCoords[3 * i + 2].u,
                                  mesh->textCoords[3 * i + 2].v);
         }
      }
      mesh = mesh->next;
   }

   exit(0);

   /// Scale object
   C_Vertex min, max;
   wallMesh.bbox.GetMax(&max);
   wallMesh.bbox.GetMin(&min);
   float xLen = max.x - min.x;
   float zLen = max.z - min.z;
   float scale = TILE_SIZE / MAX(xLen, zLen);
   ESMatrix matrix = Identity;
   esScale(&matrix, scale, scale, scale);
   if(zLen > xLen)
      esRotate(&matrix, 270.0f, 0.0f, 1.0f, 0.0f);
   wallMesh.applyTransformationOnVertices(&matrix);

   return true;
}

void
C_Map::draw(C_Camera *camera)
{
   int mapPolys;

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//   wallMesh.draw(camera);
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
			tiles[_x][_y].setParameter(buf);

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
