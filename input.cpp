#include "input.h"

C_InputHandler::C_InputHandler(void)
{
   buttonW_ = new C_MoveCommand();
   buttonW_->setMovement(MOVE_FORWARD);

   buttonS_ = new C_MoveCommand();
   buttonS_->setMovement(MOVE_BACKWARDS);

   buttonA_ = new C_MoveCommand();
   buttonA_->setMovement(MOVE_TURN_LEFT);

   buttonD_ = new C_MoveCommand();
   buttonD_->setMovement(MOVE_TURN_RIGHT);

   buttonQ_ = new C_MoveCommand();
   buttonQ_->setMovement(MOVE_STRAFE_LEFT);

   buttonE_ = new C_MoveCommand();
   buttonE_->setMovement(MOVE_STRAFE_RIGHT);
}

void
C_MoveCommand::execute(C_Actor *actor)
{
   assert(movement < MOVE_MAX_MOVES);

   actor->move(movement);

//   mob.move(MOVE_STRAFE_LEFT);
}

C_InputHandler::~C_InputHandler(void)
{
   delete buttonW_;
   delete buttonS_;
   delete buttonA_;
   delete buttonD_;
   delete buttonQ_;
   delete buttonE_;
}

void
C_InputHandler::pressKey(char key)
{
   assert(key >= 65 && key <= 119);

   keysPressed[key - 65] = true;
}

void
C_InputHandler::releaseKey(char key)
{
   assert(key >= 65 && key <= 119);
   assert(keysPressed[key - 65]);

   keysPressed[key - 65] = false;
}

bool
C_InputHandler::isPressed(char key)
{
   assert(key >= 65 && key <= 119);

   return keysPressed[key - 65];
}

C_Command *
C_InputHandler::handleInput()
{
  if (isPressed('w')) return buttonW_;
  if (isPressed('a')) return buttonA_;
  if (isPressed('s')) return buttonS_;
  if (isPressed('d')) return buttonD_;
  if (isPressed('q')) return buttonQ_;
  if (isPressed('e')) return buttonE_;

  return NULL;
}
