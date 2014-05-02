#include "actor.h"
#include "input.h"

C_Actor::C_Actor(void)
{
   mapCoordinateX = 0;
   mapCoordinateY = 0;
   map = NULL;
   facingDirection = Z_PLUS;
   movingDirection = Z_PLUS;

   moving = false;
   yAngle = 0.0f;
   cartesianCoordinates.x = 0.0f;
   cartesianCoordinates.y = 0.0f;
   cartesianCoordinates.z = 0.0f;
}

void
C_Actor::updateDirections(void)
{
//   printf("facing: %d moving: %d\n", facingDirection, movingDirection);
//
//   tile *tile_;
//
//   printf("map neighbour tiles:\n");
//
//   tile_ = map->getTile(mapCoordinateX + 1, mapCoordinateY);
//   printf("\t x+1, y: %d\n", tile_->getType());
//
//   tile_ = map->getTile(mapCoordinateX - 1, mapCoordinateY);
//   printf("\t x-1, y: %d\n", tile_->getType());
//
//   tile_ = map->getTile(mapCoordinateX, mapCoordinateY + 1);
//   printf("\t x, y+1: %d\n", tile_->getType());
//
//   tile_ = map->getTile(mapCoordinateX + 1, mapCoordinateY - 1);
//   printf("\t x, y-1: %d\n", tile_->getType());

   switch(movement) {
   case MOVE_STRAFE_LEFT:
      if(facingDirection == X_MINUS) {
         movingDirection = Z_PLUS;
      } else {
         movingDirection = (tile_direction_t)((int)facingDirection - 1);
      }
      break;

   case MOVE_STRAFE_RIGHT:
      if(facingDirection == Z_PLUS) {
         movingDirection = X_MINUS;
      } else {
         movingDirection = (tile_direction_t)((int)facingDirection + 1);
      }
      break;

   case MOVE_TURN_LEFT:
      if(facingDirection == X_MINUS) {
         facingDirection = Z_PLUS;
         movingDirection = Z_PLUS;
      } else {
         facingDirection = (tile_direction_t)((int)facingDirection - 1);
         movingDirection = (tile_direction_t)((int)movingDirection - 1);
      }
      break;

   case MOVE_TURN_RIGHT:
      if(facingDirection == Z_PLUS) {
         facingDirection = X_MINUS;
         movingDirection = X_MINUS;
      } else {
         facingDirection = (tile_direction_t)((int)facingDirection + 1);
         movingDirection = (tile_direction_t)((int)movingDirection + 1);
      }
      break;

   case MOVE_BACKWARDS:
      if(facingDirection == X_MINUS || facingDirection == Z_MINUS) {
         movingDirection = (tile_direction_t)((int)facingDirection + 2);
      } else {
         movingDirection = (tile_direction_t)((int)facingDirection - 2);
      }
      break;

   case MOVE_FORWARD:
      movingDirection = facingDirection;
      break;

   default:
      assert(0);
      break;
   }

//   printf("facing: %d moving: %d\n", facingDirection, movingDirection);
}

void
C_Actor::updateTileCoordinates(void)
{
   switch(movingDirection) {
   case X_PLUS:
      ++mapCoordinateY;
      break;
   case X_MINUS:
      --mapCoordinateY;
      break;
   case Z_PLUS:
      ++mapCoordinateX;
      break;
   case Z_MINUS:
      --mapCoordinateX;
      break;
   default:
      assert(0);
      break;
   }
}

bool
C_Actor::checkCollision(void)
{
   assert(map);
   tile *tile_;

   switch(movingDirection) {
   case X_PLUS:
      tile_ = map->getTile(mapCoordinateX, mapCoordinateY + 1);
      break;

   case X_MINUS:
      tile_ = map->getTile(mapCoordinateX, mapCoordinateY - 1);
      break;

   case Z_PLUS:
      tile_ = map->getTile(mapCoordinateX + 1, mapCoordinateY);
      break;

   case Z_MINUS:
      tile_ = map->getTile(mapCoordinateX - 1, mapCoordinateY);
      break;

   default:
      assert(0);
      break;
   }

   return tile_->getType() == TILE_WALL;
}

void
C_Actor::move(movements_t movement)
{
   if(moving)
      return;

   this->movement = movement;
   updateDirections();

   if(movement != MOVE_TURN_LEFT && movement != MOVE_TURN_RIGHT)
      if(ENABLE_COLLISION_DETECTION && checkCollision())
         return;

   moving = true;
}

void
C_Mob::loadModel(void)
{
   model.loadFromFile("objmodels/cube.obj");
   model.shader = simple_texture_shader;

   ESMatrix matrix = Identity;
   float scale = 5.0f;
   esScale(&matrix, scale, scale, scale);
   model.applyTransformationOnVertices(&matrix);
}

C_Mob::C_Mob(void)
{
   mapCoordinateX = 1;
   mapCoordinateY = 1;
}

void
C_Mob::Draw(C_Camera *camera)
{
   model.position.x = mapCoordinateX * TILE_SIZE + TILE_SIZE / 2.0f;
   model.position.y = TILE_SIZE / 4.f;
   model.position.z = mapCoordinateY * TILE_SIZE + TILE_SIZE / 2.0f;


   model.draw(camera);
}

void
C_Actor::update(float fps)
{
   static float change = 0.0f;
   static const float rotationSpeed = 220.0f * 1.0f / fps;
   static const float moveSpeed = 60.0f * 1.0f / fps;

   if(moving) {
      switch(movement) {
      case MOVE_TURN_LEFT:
         change += rotationSpeed;

         if(change >= 90.0f) {
            yAngle += rotationSpeed - (change - 90.0f);
            moving = false;
            change = 0.0f;
            movement = MOVE_MAX_MOVES;
         } else {
            yAngle += rotationSpeed;
         }
         break;

      case MOVE_TURN_RIGHT:
         change -= rotationSpeed;

         if(change <= -90.0f) {
            yAngle += -rotationSpeed - (change + 90.0f);

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            yAngle -= rotationSpeed;
         }
         break;

      case MOVE_FORWARD:
         change += moveSpeed;

         if(change >= TILE_SIZE) {
            switch(movingDirection) {
            case X_PLUS: cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            case X_MINUS: cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            case Z_PLUS: cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            case Z_MINUS: cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            default: assert(0); break;
            }

            updateTileCoordinates();

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            switch(movingDirection) {
            case X_PLUS: cartesianCoordinates.x += moveSpeed; break;
            case X_MINUS: cartesianCoordinates.x += moveSpeed; break;
            case Z_PLUS: cartesianCoordinates.z += moveSpeed; break;
            case Z_MINUS: cartesianCoordinates.z += moveSpeed; break;
            default: assert(0); break;
            }
         }
         break;

      case MOVE_BACKWARDS:
         change -= moveSpeed;

         if(change <= -TILE_SIZE) {
            switch(movingDirection) {
            case X_PLUS: cartesianCoordinates.x += -moveSpeed - (change + TILE_SIZE); break;
            case X_MINUS: cartesianCoordinates.x += -moveSpeed - (change + TILE_SIZE); break;
            case Z_PLUS: cartesianCoordinates.z += -moveSpeed - (change + TILE_SIZE); break;
            case Z_MINUS: cartesianCoordinates.z += -moveSpeed - (change + TILE_SIZE); break;
            default: assert(0); break;
            }

            updateTileCoordinates();

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            switch(movingDirection) {
            case X_PLUS: cartesianCoordinates.x -= moveSpeed; break;
            case X_MINUS: cartesianCoordinates.x -= moveSpeed; break;
            case Z_PLUS: cartesianCoordinates.z -= moveSpeed; break;
            case Z_MINUS: cartesianCoordinates.z -= moveSpeed; break;
            default: assert(0); break;
            }
         }
         break;

      case MOVE_STRAFE_LEFT:
      case MOVE_STRAFE_RIGHT:
         change += moveSpeed;

         if(change >= TILE_SIZE) {
            switch(movingDirection) {
            case X_PLUS: cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            case X_MINUS: cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            case Z_PLUS: cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            case Z_MINUS: cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            default: assert(0); break;
            }

            updateTileCoordinates();

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            switch(movingDirection) {
            case X_PLUS: cartesianCoordinates.x += moveSpeed; break;
            case X_MINUS: cartesianCoordinates.x += moveSpeed; break;
            case Z_PLUS: cartesianCoordinates.z += moveSpeed; break;
            case Z_MINUS: cartesianCoordinates.z += moveSpeed; break;
            default: assert(0); break;
            }
         }
         break;

      default:
         assert(0);
         break;
      }
   }
}

void
C_Party::update(float fps)
{
   C_Command *command = inputHandler.handleInput();

   if(command) {
      command->execute(this);
   }

   if(moving) {
      float yAngle_old;
      C_Vertex cartesianCoordinates_old;

      switch(movement) {
      case MOVE_TURN_LEFT:
         yAngle_old = yAngle;
         C_Actor::update(fps);
         camera.Rotate(0.0f, yAngle - yAngle_old);
         break;

      case MOVE_TURN_RIGHT:
         yAngle_old = yAngle;
         C_Actor::update(fps);
         camera.Rotate(0.0f, yAngle - yAngle_old);
         break;

      case MOVE_FORWARD:
      case MOVE_BACKWARDS:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == X_PLUS || movingDirection == X_MINUS)
            camera.Move(cartesianCoordinates.x - cartesianCoordinates_old.x);
         else
            camera.Move(cartesianCoordinates.z - cartesianCoordinates_old.z);
         break;

      case MOVE_STRAFE_LEFT:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == X_PLUS || movingDirection == X_MINUS)
            camera.StrafeLeft(cartesianCoordinates.x - cartesianCoordinates_old.x);
         else
            camera.StrafeLeft(cartesianCoordinates.z - cartesianCoordinates_old.z);
         break;

      case MOVE_STRAFE_RIGHT:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == X_PLUS || movingDirection == X_MINUS)
            camera.StrafeRight(cartesianCoordinates.x - cartesianCoordinates_old.x);
         else
            camera.StrafeRight(cartesianCoordinates.z - cartesianCoordinates_old.z);
         break;

      default:
         assert(0);
         break;
      }
   }
}
