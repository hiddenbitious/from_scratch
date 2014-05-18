#include "battleStaticObject.h"

C_BattleStaticObject::C_BattleStaticObject(C_BattleMap *battleMap, float height, bool walkable) :
C_BattleObject(battleMap, height)
{
   m_walkable = walkable;
}
