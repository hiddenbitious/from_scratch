#ifndef _BATTLESTATICOBJECT_H_
#define _BATTLESTATICOBJECT_H_

#include "battleObject.h"

class C_BattleStaticObject : public C_BattleObject {
public:
   C_BattleStaticObject(C_BattleMap *battleMap, float height, bool walkable);

   int GetGridX();
   int GetGridY();
   int GetGridZ();

   bool IsWalkable();

protected:
   C_BattleStaticObject(C_BattleMap *battleMap, float h);

private:
   bool m_walkable;
};

#endif
