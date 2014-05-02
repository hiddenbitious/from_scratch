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

   virtual void update(int fps);

protected:
   int mapCoordinateX;
   int mapCoordinateY;
   float change;

   float yAngle;
   C_Vertex cartesianCoordinates;

   tile_direction_t facingDirection;
   tile_direction_t movingDirection;
   movements_t movement;
   bool moving;

   void updateDirections(void);
   void updateTileCoordinates(void);
   bool checkCollision(void);

   C_Map *map;
};

class C_Mob : public C_Actor {
private:
   C_MeshGroup model;

public:
   C_Mob(void);
   ~C_Mob(void) {}

   void loadModel(void);

   virtual void update(int fps);
   void Draw(C_Camera *camera);

};

class C_Party : public C_Actor {
public:
   C_Party(void) {}
   ~C_Party(void) {}

   virtual void update(int fps);
};

#endif
