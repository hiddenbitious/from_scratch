#ifndef _BATTLEPLAYER_H_
#define _BATTLEPLAYER_H_

#include "../globals.h"
#include "battleMap.h"

class C_BattlePlayer : public C_BattleDynamicObject
{
public:
   C_BattlePlayer(C_BattleMap *battleMap);

   virtual void Draw();
   virtual void Update(float time);
   virtual void OnNotify(unsigned const int type);

private:
   void OnMouseLeftDown();
};

#endif
