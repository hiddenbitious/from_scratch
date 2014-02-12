#include <stdio.h>
#include <vector>
#include <string.h>

#include "map.h"
float tileSize;

C_Map::~C_Map(void)
{
   if(mergedTiles.size())
      mergedTiles.clear();
}

bool
C_Map::readMap(const char *filename)
{
	int _xTiles, _yTiles, _tileSize, tmp, nTiles, _x, _y;
	int i, c, sum = 0;
	char buf[MAX_PARAMETER_LENGTH];
	int counters[N_TILE_TYPES] = {0};

	FILE *fd;

	if((fd = fopen(filename, "r")) == NULL)
		return false;

	fscanf(fd, "%d", &_xTiles);		/// Read size on x.
	fscanf(fd, "%d", &_yTiles);		/// Read size on y.
	fscanf(fd, "%d", &nTiles);		   /// Read number of tiles stored in file.
	fscanf(fd, "%f", &_tileSize);	   /// Read tile size (not used).

   assert(_xTiles == TILES_ON_X);
   assert(_yTiles == TILES_ON_Y);
   assert(nTiles <= TILES_ON_X * TILES_ON_Y);

	for(i = 0; i < nTiles; i++) {
		fscanf(fd, "%d", &_x);			/// Read x coords
		fscanf(fd, "%d", &_y);			/// Read y coords
		c = fscanf(fd, "%d", &tmp);	/// Read tile type
		assert(_x < TILES_ON_X);
		assert(_y < TILES_ON_Y);
		assert(tmp <= MAX_TILE_TYPES);

		tiles[_x][_y].setType(tmp);
		tiles[_x][_y].setCoordX(_x);
		tiles[_x][_y].setCoordY(_y);

		/// Count tiles
		++counters[tmp];
		++sum;

   	/// There's a parameter. Read it.
		if(!c) {
		   /// fgets doesn't stop at white spaces but it stops at '\n'
			fgets(buf, MAX_PARAMETER_LENGTH, fd);
			tiles[_x][_y].setParameter(buf);
		   /// One loop is lost everytime a parameter is read.
			--i;
		}
	}

	fclose(fd);

   printf("\n*****\n");
	printf("Read map %s. Found:\n", filename);
	for(int i = 0; i < N_TILE_TYPES; i++) {
	   if(counters[i])
         printf("\tType %d: %d tiles\n", i, counters[i]);
	}
	printf("\tTotal %d tiles\n", sum);
   printf("*****\n");

   mergeTiles();

	return true;
}

void
C_Map::secondPass(void)
{
   vector<mergedTile_t>::iterator itx;
   vector<mergedTile_t>::iterator ity;

   for(itx = mergedTiles.begin(); itx < mergedTiles.end(); itx++) {
      for(ity = mergedTiles.begin(); ity < mergedTiles.end(); ity++) {
         /// Merge rows
         if((*itx).y + (*itx).height == (*ity).y &&
            (*itx).x == (*ity).x &&
            (*itx).width == (*ity).width) {
            (*itx).height += (*ity).height;
            mergedTiles.erase(ity);
            --itx;
         } /// Merge collums
         else if((*itx).x + (*itx).width == (*ity).x &&
            (*itx).y == (*ity).y &&
            (*itx).height == (*ity).height) {
            (*itx).width += (*ity).width;
            mergedTiles.erase(ity);
            --itx;
         }
      }
   }
}

void
C_Map::firstPass(int x, int y, bool **visitedTiles)
{
   int xx = x, yy = y;
   int width = 1, height = 1;
   mergedTile_t merged = {x, y, width, height};

   /// Mark start tile as visited
   visitedTiles[xx][yy] = true;

   /// Scan row
   if(yy == TILES_ON_Y - 1 || tiles[xx + 1][yy].getType() == TILE_WALL) {
      while(tiles[xx + 1][yy].getType() == TILE_WALL && xx <= TILES_ON_X - 2) {
         xx++;
         width++;
         visitedTiles[xx][yy] = true;
      }

      merged.width = width;
   } else if(xx == TILES_ON_X - 1 || tiles[xx][yy + 1].getType() == TILE_WALL) {
      /// ... else scan the column
      while(tiles[xx][yy + 1].getType() == TILE_WALL && yy <= TILES_ON_Y - 2) {
         yy++;
         height++;
         visitedTiles[xx][yy] = true;
      }

      merged.height = height;
   }

   mergedTiles.push_back(merged);
}

void
C_Map::mergeTiles(void)
{
   int x, y, wallTiles = 0, wallTiles2;

   /// Erase vector
   if(mergedTiles.size())
      mergedTiles.clear();

   /// initialize
   bool **visitedTiles = new bool *[TILES_ON_X];
   for(x = 0; x < TILES_ON_X; x++) {
      visitedTiles[x] = new bool[TILES_ON_Y];
      memset((void *)visitedTiles[x], false, sizeof(bool) * TILES_ON_Y);
   }

   /// Count wall tiles
   for(x = 0; x < TILES_ON_X; x++) {
      for(y = 0; y < TILES_ON_Y; y++) {
         if(tiles[x][y].getType() == TILE_WALL) wallTiles++;
      }
   }

   /// Scan all tiles
   for(x = 0; x < TILES_ON_X; x++) {
      for(y = 0; y < TILES_ON_Y; y++) {
         if(tiles[x][y].getType() == TILE_WALL && visitedTiles[x][y] == false)
            firstPass(x, y, visitedTiles);
      }
   }

   wallTiles2 = mergedTiles.size();

   /// Print merged tiles after 1st pass
//   for(x = 0; x < mergedTiles.size(); ++x)
//      printf("%d: (%d, %d): %d x %d\n", x, mergedTiles[x].x, mergedTiles[x].y, mergedTiles[x].width, mergedTiles[x].height);

   /// Try to merge more
   secondPass();

   /// Print merged tiles after 2nd pass
//   printf("\n\n");
//   for(x = 0; x < mergedTiles.size(); ++x)
//      printf("%d: (%d, %d): %d x %d\n", x, mergedTiles[x].x, mergedTiles[x].y, mergedTiles[x].width, mergedTiles[x].height);

   printf("\n*****\n");
   printf("Map merged:\n");
   printf("\tInitial wall tiles: %d\n", wallTiles);
   printf("\tWall tiles after 1st pass: %d\n", wallTiles2);
   printf("\tWall tiles after 2nd pass: %d\n", mergedTiles.size());
   printf("*****\n");

   for(x = 0; x < TILES_ON_X; x++)
      delete[] visitedTiles[x];
   delete[] visitedTiles;
}
