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

void C_BattleMap::AddDynamicObject(C_BattleDynamicObject *o)
{
//    o->init(&pathfinder);
    m_dynamicObjectList.push_back(o);
}

void C_BattleMap::AddStaticObject(C_BattleStaticObject *o)
{
//    o->init();
    m_staticObjectList.push_back(o);
//    pathfinder.addStaticObject(o);
}
