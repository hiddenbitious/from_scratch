#ifndef _MAP_H_
#define _MAP_H_

#include "globals.h"
#include "tile.h"

#define MAX_PARAMETER_LENGTH			256   /// Maximum length of a parameter.
#define MAX_TILE_TYPES                8   /// Number of different tile types
#define TILES_ON_X                   80
#define TILES_ON_Y                   50

typedef struct {
   /// Tile center
   int x, y;
   /// Tile size
   int width, height;
} mergedTile_t;

class C_Map {
   friend class C_BspTree;

public:
   C_Map(void) {};
   ~C_Map(void);
   bool readMap(const char *filename);
   void mergeTiles(void);
//   void Draw(void);

private:
   float tileSize;
   tile tiles[80][50];
   vector<mergedTile_t> mergedTiles;

   void firstPass(int x, int y, bool **visitedTiles);
   void secondPass(void);
};

#endif
