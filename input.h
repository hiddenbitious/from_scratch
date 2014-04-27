#ifndef _INPUT_H_
#define _INPUT_H_

#include "globals.h"
#include "actor.h"

class C_Command
{
public:
   virtual ~C_Command() {}
   virtual void execute(C_Actor *actor) = 0;
};

class C_MoveCommand : public C_Command {
private:
   movements_t movement;

public:
   C_MoveCommand(void) { movement = MOVE_MAX_MOVES; }
   void setMovement(movements_t movement) { this->movement = movement; }

   virtual void execute(C_Actor *actor);
};


class C_InputHandler
{
public:
   C_InputHandler(void);

   C_Command *handleInput();

   void pressKey(char key);
   void releaseKey(char key);
   bool isPressed(char key);

private:
   C_MoveCommand *buttonW_;
   C_MoveCommand *buttonS_;
   C_MoveCommand *buttonA_;
   C_MoveCommand *buttonD_;
   C_MoveCommand *buttonQ_;
   C_MoveCommand *buttonE_;

   bool keysPressed[54];
};

#endif
