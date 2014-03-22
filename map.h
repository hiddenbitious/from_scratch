#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"
#include "bspTree.h"
#include "globals.h"

#define TILE_SIZE 20.0f

class C_Map {

public:
   C_Map(void);
   ~C_Map(void) {};

   bool createMap(const char *filename);

   void draw(C_Camera *camera);

private:
   C_BspTree *bspTree;
   C_MeshGroup wallMesh;


   /// All map tiles
   tile tiles[TILES_ON_X][TILES_ON_Y];

   bool readMap(const char *filename);
   bool load3DObjects(void);
   bool placeObjects(void);
};

#endif
