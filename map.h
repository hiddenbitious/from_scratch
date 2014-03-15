#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"
#include "bspTree.h"

class C_Map {

public:
   C_Map(void) {};
   ~C_Map(void) {};

   bool readMap(const char *filename);
   bool readMapAreas(const char *filename);
   bool createMap(void);

   void draw(void);

private:
   C_BspTree *bspTree;

   /// All map tiles
   tile tiles[TILES_ON_X][TILES_ON_Y];

};

#endif
