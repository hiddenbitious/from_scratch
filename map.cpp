#include <stdio.h>
#include "map.h"

bool
C_Map::readMap(const char *filename)
{
   printf("*********\n");
   printf("Reading map file \"%s\"\n", filename);

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
		assert(tmp < N_TILE_TYPES);

		tiles[_x][_y].setType((tileTypes_t)tmp);
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
         --counters[tmp];
		}
	}

	fclose(fd);

	printf("\tFound:\n");
	for(int i = 0; i < N_TILE_TYPES; i++) {
	   if(counters[i])
         printf("\t\tType %d: %d tiles\n", i, counters[i]);
	}
	printf("\tTotal %d tiles\n", sum);
	printf("Done.");
   printf("*********\n");

	return true;
}

bool
C_Map::readMapAreas(const char *filename)
{
   printf("*********\n");
   printf("Reading map areas from file \"%s\"\n", filename);

   int _xTiles, _yTiles, type, _x, _y;
   areaTypes_t area;

   FILE *fd;

   if((fd = fopen(filename, "r")) == NULL) {
      assert(0);
      return false;
   }

   fscanf(fd, "%d", &_xTiles);		/// Read size on x.
   fscanf(fd, "%d", &_yTiles);		/// Read size on y.
   assert(_xTiles == TILES_ON_X);
   assert(_yTiles == TILES_ON_Y);

   for(int i = 0; i < TILES_ON_X * TILES_ON_Y; i++) {
      fscanf(fd, "%d", &_x);			/// Read x coords
      fscanf(fd, "%d", &_y);			/// Read y coords
      fscanf(fd, "%d", &type);	   /// Read tile type
      fscanf(fd, "%d", &area);	   /// Read tile area

      assert(_x < TILES_ON_X);
      assert(_y < TILES_ON_Y);
      assert(type < N_TILE_TYPES);

      tiles[_x][_y].setArea(area);
   }

   return true;
}
