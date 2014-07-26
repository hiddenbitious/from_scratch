#ifndef _BATTLEMAP_H_
#define _BATTLEMAP_H_

#include <list>

#include "../globals.h"
#include "../camera.h"
#include "battleDynamicObject.h"
#include "battleStaticObject.h"

extern bool DEBUG;

extern int WORLD_WIDTH;
extern int WORLD_HEIGHT;

extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;

class C_BattleDynamicObject;
class C_BattleStaticObject;

class C_BattleMap
{
public:
   C_BattleMap(C_Camera *camera);
   virtual ~C_BattleMap(void);

   void Draw(void);

   void AddDynamicObject(C_BattleDynamicObject *o);
   void AddStaticObject(C_BattleStaticObject *o);

//   void SetMousePosition(unsigned int x, unsigned int y);
//   void SetFocus(C_BattleDynamicObject *o);
//   void UpdateMousePosition(float mouseScreenX, float mouseScreenY);

   float m_MouseX;
   float m_MouseY;
   C_Camera *m_camera;

private:
   std::list<C_BattleDynamicObject *> m_dynamicObjectList;
   std::list<C_BattleStaticObject *>  m_staticObjectList;

//   Pathfinder pathfinder;
};

#endif
