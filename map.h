#ifndef _MAP_H_
#define _MAP_H_

#include "globals.h"
#include "tile.h"

#define MAX_PARAMETER_LENGTH			256   /// Maximum length of a parameter.
#define MAX_TILE_TYPES                8   /// Number of different tile types
#define TILES_ON_X                   80
#define TILES_ON_Y                   50

class C_Map {
public:
   C_Map(void) {};
   bool readMap(const char *filename);
   void Draw(void);

private:
   float tileSize;
   tile tiles[80][50];							   /// Holds the tile data

};

#endif
