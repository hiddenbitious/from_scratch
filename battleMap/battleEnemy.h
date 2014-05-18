#ifndef _BATTLEENEMY_H_
#define _BATTLEENEMY_H_

#include "../globals.h"
#include "battleMap.h"

class C_BattleEnemy : public C_BattleDynamicObject
{
public:

   C_BattleEnemy(C_BattleMap *battleMap);

   virtual void Draw();
   virtual void Update(float time);
};

#endif
