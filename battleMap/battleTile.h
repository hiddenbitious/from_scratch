#ifndef _BATTLETILE_H_
#define _BATTLETILE_H_

#include "battleStaticObject.h"

class C_BattleTile: public C_BattleStaticObject
{
public:
   C_BattleTile(C_BattleMap *battleMap, float x, float y, float z, float height);
   virtual ~C_BattleTile(void) {}
//   virtual void Draw();
};

#endif
