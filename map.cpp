#include <stdio.h>
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

bool
C_Map::createMap(const char *filename)
{
   bool ret;

   printf("Creating map...\n");

   ret = readMap(filename);
   if(!ret) {
      assert(0);
      return false;
   }

   /// Load 3d meshes
   load3DObjects();

   /// Position the walls
   placeObjects();

   bspTree = new C_BspTree(6);

   bspTree->ReadGeometryFile("mapGeometry.bsp");
   bspTree->BuildBspTree();
   bspTree->BuildPVS();

   return true;
}

bool
C_Map::placeObjects(void)
{
   ESMatrix mat;
   esMatrixLoadIdentity(&mat);
   esRotate(&mat, 90.0f, 0.0f, 1.0f, 0.0f);

   C_MeshGroup copiesOfWall[3];
   copiesOfWall[0] = wallMesh;
   copiesOfWall[0].applyTransformationOnVertices(&mat);

   copiesOfWall[1] = copiesOfWall[0];
   copiesOfWall[1].applyTransformationOnVertices(&mat);

   copiesOfWall[2] = copiesOfWall[1];
   copiesOfWall[2].applyTransformationOnVertices(&mat);

   /// Position the walls into the bsp tree!
   for(int x = 0; x < TILES_ON_X; x++) {
      for(int y = 0; y < TILES_ON_Y; y++) {

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

   wallMesh.loadFromFile("objmodels/fence.obj");
   wallMesh.shader = wallShader;
return true;
}

void
C_Map::draw(C_Camera *camera)
{
   int mapPolys;

   C_MeshGroup copyOfWall;
   copyOfWall = wallMesh;
   ESMatrix mat;
   esMatrixLoadIdentity(&mat);
   esRotate(&mat, 90.0f, 0.0f, 1.0f, 0.0f);
   copyOfWall.applyTransformationOnVertices(&mat);

   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

   copyOfWall.draw(camera);

   mapPolys = bspTree->Draw_PVS(camera);
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
