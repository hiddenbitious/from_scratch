#include "battleObject.h"

C_BattleObject::C_BattleObject(C_BattleMap *battleMap, float height)
{
   m_battleMap = battleMap;
   m_height = height;
   m_size = 32.0f;
}

void
C_BattleObject::SetPosition(float x, float y, float z)
{
   m_position.x = x;
   m_position.y = y;
   m_position.z = z;
}
