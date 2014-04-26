#ifndef _ACTOR_H_
#define _ACTOR_H_

#include "globals.h"
#include "map.h"

typedef enum {MOVE_FORWARD,
              MOVE_BACKWARDS,
              MOVE_TURN_RIGHT,
              MOVE_TURN_LEFT,
              MOVE_STRAFE_RIGHT,
              MOVE_STRAFE_LEFT,
              MOVE_MAX_MOVES} movements_t;

class C_Actor {
public:
   C_Actor(void);
   ~C_Actor(void) {}

   void move(movements_t movement);
   void setMap(C_Map *map_) { assert(map_); map = map_; }
   void setCoordinates(int x, int y) { mapCoordinateX = x; mapCoordinateY = y; }

   void update();

protected:
   int mapCoordinateX;
   int mapCoordinateY;
   tile_direction_t facingDirection;
   tile_direction_t movingDirection;
   bool moving;
   movements_t movement;

   void updateDirections(void);
   void updateTileCoordinates(void);
   bool checkCollision(void);

   C_Map *map;
};

class C_Party : public C_Actor {
public:
   C_Party(void) {}
   ~C_Party(void) {}
};

#endif
