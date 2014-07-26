#ifndef _BATTLEOBJECT_H_
#define _BATTLEOBJECT_H_

#include "../globals.h"
#include "../mesh.h"

class C_BattleMap;

class C_BattleObject {
public:
   C_BattleObject(C_BattleMap *battleMap, float height);

   void Init();
   virtual void Draw(void) { model.draw(&camera); }

   void SetPosition(float x, float y, float z);

   inline float GetX(void) { return m_position.x; }
   inline float GetY(void) { return m_position.y; }
   inline float GetZ(void) { return m_position.z; }
   inline C_Vertex GetPosition(void)  { return m_position; }
   inline float GetHeight(void) { return m_height; }
   inline float GetSize(void) { return m_size; }

protected:
   C_BattleMap *m_battleMap;
   C_Vertex    m_position;
   C_MeshGroup model;

private:
   float       m_height;
   float       m_size;
};

#endif
