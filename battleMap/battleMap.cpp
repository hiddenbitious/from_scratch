#include "battleMap.h"

bool DEBUG = true;

int WORLD_WIDTH      = 2048;
int WORLD_HEIGHT     = 2048;

float SCREEN_WIDTH   = 800;
float SCREEN_HEIGHT  = 600;


C_BattleMap::C_BattleMap(C_Camera *camera)
{
   m_camera = camera;
}

C_BattleMap::~C_BattleMap(void)
{
   for(list<C_BattleStaticObject *>::iterator i = m_staticObjectList.begin(); i != m_staticObjectList.end(); ++i) {
      delete (*i);
   }
}

void
C_BattleMap::AddDynamicObject(C_BattleDynamicObject *o)
{
//    o->init(&pathfinder);
    m_dynamicObjectList.push_back(o);
}

void
C_BattleMap::AddStaticObject(C_BattleStaticObject *o)
{
//    o->init();
    m_staticObjectList.push_back(o);
//    pathfinder.addStaticObject(o);
}

void
C_BattleMap::Draw(void)
{
   for(list<C_BattleStaticObject *>::iterator i = m_staticObjectList.begin(); i != m_staticObjectList.end(); ++i) {
      (*i)->Draw();
   }

   for(list<C_BattleDynamicObject *>::iterator i = m_dynamicObjectList.begin(); i != m_dynamicObjectList.end(); ++i) {
      (*i)->Draw();
   }
}
