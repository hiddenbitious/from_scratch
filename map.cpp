#include <stdio.h>

#include "map.h"
float tileSize;

bool
C_Map::readMap(const char *filename)
{
	int _xTiles, _yTiles, _tileSize, tmp, nTiles, _x, _y;
	int i, c;
	char buf[MAX_PARAMETER_LENGTH];

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
	return true;
}
