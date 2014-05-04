#include "actor.h"
#include "input.h"

C_Actor::C_Actor(void)
{
   mapCoordinateX = 0;
   mapCoordinateY = 0;
   map = NULL;
   facingDirection = TILE_X_PLUS;
   movingDirection = TILE_X_PLUS;

   moving = false;
   yAngle = 0.0f;
   change = 0.0f;
   cartesianCoordinates.x = 0.0f;
   cartesianCoordinates.y = 0.0f;
   cartesianCoordinates.z = 0.0f;
}

void
C_Actor::setCoordinates(int x, int y)
{
   mapCoordinateX = x;
   mapCoordinateY = y;

   cartesianCoordinates.x = y * TILE_SIZE + TILE_SIZE / 2.0f;
   cartesianCoordinates.y = TILE_SIZE / 2.0f;
   cartesianCoordinates.z = x * TILE_SIZE + TILE_SIZE / 2.0f;
}

void
C_Actor::updateDirections(void)
{
   switch(movement) {
   case MOVE_STRAFE_LEFT:
      movingDirection = (tile_direction_t)(((int)facingDirection + 1) % TOTAL_TILE_DIRECTIONS);
      break;

   case MOVE_STRAFE_RIGHT:
      movingDirection = (tile_direction_t)(((int)facingDirection + TOTAL_TILE_DIRECTIONS - 1) % TOTAL_TILE_DIRECTIONS);
      break;

   case MOVE_TURN_LEFT:
      facingDirection = movingDirection = (tile_direction_t)(((int)facingDirection + 1) % TOTAL_TILE_DIRECTIONS);
      break;

   case MOVE_TURN_RIGHT:
      facingDirection = movingDirection = (tile_direction_t)(((int)facingDirection + TOTAL_TILE_DIRECTIONS - 1) % TOTAL_TILE_DIRECTIONS);
      break;

   case MOVE_BACKWARDS:
      movingDirection = (tile_direction_t)(((int)facingDirection + 2) % TOTAL_TILE_DIRECTIONS);
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
   case TILE_X_PLUS:
      ++mapCoordinateX;
      break;
   case TILE_X_MINUS:
      --mapCoordinateX;
      break;
   case TILE_Y_PLUS:
      ++mapCoordinateY;
      break;
   case TILE_Y_MINUS:
      --mapCoordinateY;
      break;
   default:
      assert(0);
      break;
   }

//   printf("coordx: %d coordy: %d\n\n", mapCoordinateX, mapCoordinateY);
}

bool
C_Actor::checkCollision(void)
{
   assert(map);
   tile *tile_;

   switch(movingDirection) {
   case TILE_X_PLUS:
      tile_ = map->getTile(mapCoordinateX + 1, mapCoordinateY);
//      printf("X_PLUS tile type: %d\n", tile_->getType());
      break;

   case TILE_X_MINUS:
      tile_ = map->getTile(mapCoordinateX - 1, mapCoordinateY);
//      printf("X_MINUS tile type: %d\n", tile_->getType());
      break;

   case TILE_Y_PLUS:
      tile_ = map->getTile(mapCoordinateX, mapCoordinateY + 1);
//      printf("Z_PLUS tile type: %d\n", tile_->getType());
      break;

   case TILE_Y_MINUS:
      tile_ = map->getTile(mapCoordinateX, mapCoordinateY - 1);
//      printf("Z_MINUS tile type: %d\n", tile_->getType());
      break;

   default:
      assert(0);
      break;
   }

//   assert(tile_->getType() != TILE_WALL);

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
C_Actor::update(int fps)
{
   const float rotationSpeed = 220.0f * 1.0f / (float)fps;
   const float moveSpeed = 60.0f * 1.0f / (float)fps;

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
            case TILE_X_PLUS:  cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            case TILE_X_MINUS: cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            case TILE_Y_PLUS:  cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            case TILE_Y_MINUS: cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            default: assert(0); break;
            }

            updateTileCoordinates();

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            switch(movingDirection) {
            case TILE_X_PLUS:  cartesianCoordinates.z += moveSpeed; break;
            case TILE_X_MINUS: cartesianCoordinates.z += moveSpeed; break;
            case TILE_Y_PLUS:  cartesianCoordinates.x += moveSpeed; break;
            case TILE_Y_MINUS: cartesianCoordinates.x += moveSpeed; break;
            default: assert(0); break;
            }
         }
         break;

      case MOVE_BACKWARDS:
         change -= moveSpeed;

         if(change <= -TILE_SIZE) {
            switch(movingDirection) {
            case TILE_X_PLUS:  cartesianCoordinates.z += -moveSpeed - (change + TILE_SIZE); break;
            case TILE_X_MINUS: cartesianCoordinates.z += -moveSpeed - (change + TILE_SIZE); break;
            case TILE_Y_PLUS:  cartesianCoordinates.x += -moveSpeed - (change + TILE_SIZE); break;
            case TILE_Y_MINUS: cartesianCoordinates.x += -moveSpeed - (change + TILE_SIZE); break;
            default: assert(0); break;
            }

            updateTileCoordinates();

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            switch(movingDirection) {
            case TILE_X_PLUS:  cartesianCoordinates.z -= moveSpeed; break;
            case TILE_X_MINUS: cartesianCoordinates.z -= moveSpeed; break;
            case TILE_Y_PLUS:  cartesianCoordinates.x -= moveSpeed; break;
            case TILE_Y_MINUS: cartesianCoordinates.x -= moveSpeed; break;
            default: assert(0); break;
            }
         }
         break;

      case MOVE_STRAFE_LEFT:
      case MOVE_STRAFE_RIGHT:
         change += moveSpeed;

         if(change >= TILE_SIZE) {
            switch(movingDirection) {
            case TILE_X_PLUS:  cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            case TILE_X_MINUS: cartesianCoordinates.z += moveSpeed - (change - TILE_SIZE); break;
            case TILE_Y_PLUS:  cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            case TILE_Y_MINUS: cartesianCoordinates.x += moveSpeed - (change - TILE_SIZE); break;
            default: assert(0); break;
            }

            updateTileCoordinates();

            moving = false;
            movement = MOVE_MAX_MOVES;
            change = 0.0f;
         } else {
            switch(movingDirection) {
            case TILE_X_PLUS:  cartesianCoordinates.z += moveSpeed; break;
            case TILE_X_MINUS: cartesianCoordinates.z += moveSpeed; break;
            case TILE_Y_PLUS:  cartesianCoordinates.x += moveSpeed; break;
            case TILE_Y_MINUS: cartesianCoordinates.x += moveSpeed; break;
            default: assert(0); break;
            }
         }
         break;

      default:
         assert(0);
         break;
      }
   }

//   printf("cartesianCoordinates: %f %f %f\n", cartesianCoordinates.x, cartesianCoordinates.y, cartesianCoordinates.z);
}

void
C_Party::update(int fps)
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
      case MOVE_TURN_RIGHT:
         yAngle_old = yAngle;
         C_Actor::update(fps);
         camera.Rotate(0.0f, yAngle - yAngle_old);
         break;

      case MOVE_FORWARD:
      case MOVE_BACKWARDS:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == TILE_X_PLUS || movingDirection == TILE_X_MINUS)
            camera.Move(cartesianCoordinates.z - cartesianCoordinates_old.z);
         else
            camera.Move(cartesianCoordinates.x - cartesianCoordinates_old.x);
         break;

      case MOVE_STRAFE_LEFT:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == TILE_X_PLUS || movingDirection == TILE_X_MINUS)
            camera.StrafeLeft(cartesianCoordinates.z - cartesianCoordinates_old.z);
         else
            camera.StrafeLeft(cartesianCoordinates.x - cartesianCoordinates_old.x);
         break;

      case MOVE_STRAFE_RIGHT:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == TILE_X_PLUS || movingDirection == TILE_X_MINUS)
            camera.StrafeRight(cartesianCoordinates.z - cartesianCoordinates_old.z);
         else
            camera.StrafeRight(cartesianCoordinates.x - cartesianCoordinates_old.x);
         break;

      default:
         assert(0);
         break;
      }
   }
}

void
C_Party::setCoordinates(int x, int y)
{
   camera.SetPosition(y * TILE_SIZE + TILE_SIZE / 2.0f,
                      TILE_SIZE / 2.0f,
                      x * TILE_SIZE + TILE_SIZE / 2.0f);

   C_Actor::setCoordinates(x, y);
}

C_Mob::C_Mob(void)
{
   mapCoordinateX = 1;
   mapCoordinateY = 1;
}

void
C_Mob::setCoordinates(int x, int y)
{
   model.translate(y * TILE_SIZE + TILE_SIZE / 2.0f - cartesianCoordinates.x,
                   0.0f,
                   x * TILE_SIZE + TILE_SIZE / 2.0f - cartesianCoordinates.y);

   C_Actor::setCoordinates(x, y);
}

void
C_Mob::loadModel(void)
{
   model.loadFromFile("objmodels/cube.obj");
   model.shader = simple_texture_shader;

   ESMatrix matrix = Identity;
   float scale = 5.0f;
   esTranslate(&matrix, 0.0f, TILE_SIZE / 4.0f, 0.0f);
   esScale(&matrix, scale, scale, scale);
   model.applyTransformationOnVertices(&matrix);
}

void
C_Mob::Draw(C_Camera *camera)
{
//   model.position = cartesianCoordinates;

   model.draw(camera);
}

void
C_Mob::update(int fps)
{
   if(moving) {
      float yAngle_old;
      C_Vertex cartesianCoordinates_old;

      switch(movement) {
      case MOVE_TURN_LEFT:
      case MOVE_TURN_RIGHT:
         yAngle_old = yAngle;
         C_Actor::update(fps);
         model.rotate(0.0f, yAngle - yAngle_old, 0.0f);
         break;

      case MOVE_FORWARD:
      case MOVE_BACKWARDS:
      case MOVE_STRAFE_LEFT:
      case MOVE_STRAFE_RIGHT:
         cartesianCoordinates_old = cartesianCoordinates;
         C_Actor::update(fps);
         if(movingDirection == TILE_X_PLUS || movingDirection == TILE_X_MINUS)
            model.translate(0.0f, 0.0f, cartesianCoordinates.z - cartesianCoordinates_old.z);
         else
            model.translate(cartesianCoordinates.x - cartesianCoordinates_old.x, 0.0f, 0.0f);
         break;

      default:
         assert(0);
         break;
      }
   }
}
