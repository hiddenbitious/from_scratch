#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"
#include "bspTree.h"
#include "globals.h"

class C_Map {

public:
   C_Map(void);
   ~C_Map(void) {};

   bool createMap(const char *filename);

   void draw(void);

private:
   C_BspTree *bspTree;
   /// All map tiles
   tile tiles[TILES_ON_X][TILES_ON_Y];

   bool readMap(const char *filename);
};

#endif
