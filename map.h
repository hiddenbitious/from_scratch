#ifndef _MAP_H_
#define _MAP_H_

#include "tile.h"
#include "bspTree.h"
#include "globals.h"

#define TILE_SIZE 20.0f

typedef enum {
   TILE_X_MINUS,
   TILE_Y_MINUS,
   TILE_X_PLUS,
   TILE_Y_PLUS,

   TOTAL_TILE_DIRECTIONS
} tile_direction_t;

class C_Map {

public:
   C_Map(void);
   ~C_Map(void);

   bool createMap(const string &filename);

   C_Vertex cameraStartPosition(int *x, int *y);
   tile *getTile(int x, int y);

   void draw(C_Camera *camera);

private:
   C_BspTree *bspTree;
//   C_MeshGroup wallMesh;
//   C_MeshGroup wallMesh2;
//   C_MeshGroup floorMesh;
//   C_MeshGroup floorMesh2;
//   C_MeshGroup floorMesh3;
//   C_MeshGroup floorMesh4;
//   C_MeshGroup grating;
//   C_MeshGroup corner_inner;
//   C_MeshGroup corner_outer;

   /// All map tiles
   tile tiles[TILES_ON_X][TILES_ON_Y];

   bool readMap(const char *filename);
   bool load3DObjects(void);
   bool placeObjects(void);
};

#endif
