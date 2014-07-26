#ifndef _BATTLEDYNAMICOBJECT_H_
#define _BATTLEDYNAMICOBJECT_H_

#include <vector>

#include "../globals.h"
#include "battleObject.h"

class C_BattleDynamicObject : public C_BattleObject {
public:
   C_BattleDynamicObject(C_BattleMap *battleMap, float h);
   virtual ~C_BattleDynamicObject(void) {}

   virtual void Draw(void);
//   virtual void Update(float time) { }
//   virtual void Remove();
//   virtual void OnNotify(unsigned const int type);

//   void Init(Pathfinder *p);
//   void Move(float time);
//   void SetDestination(C_Vertex *d);
//   void SetPath(float x, float z);
//   void ResetDestination();

protected:
   inline bool IsCanMove(void) { return m_canMove; };
   float m_speed;

private:
   std::vector<C_Vertex *> m_path;

//   Pathfinder *pathfinder;

   C_Vertex m_destination;
   C_Vertex m_direction;

   bool m_canMove;
};

#endif
