#ifndef _BATTLESTATICOBJECT_H_
#define _BATTLESTATICOBJECT_H_

#include "battleObject.h"

class C_BattleStaticObject : public C_BattleObject {
public:
   C_BattleStaticObject(C_BattleMap *battleMap, float height, bool walkable);
   virtual ~C_BattleStaticObject(void) {}

   virtual void Draw(void);
//   virtual void Updateid(float time) { }

//   int GetGridX();
//   int GetGridY();
//   int GetGridZ();

   inline bool IsWalkable(void) { return m_walkable; }

protected:
   C_BattleStaticObject(C_BattleMap *battleMap, float h);

private:
   bool m_walkable;
};

#endif
