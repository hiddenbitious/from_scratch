#include "sound.h"

static const char *soundFiles[SOUNDS] = {"audio/intro.ogg",
                                         "audio/step.ogg",
                                         "audio/rush.ogg",
                                         "audio/turn.ogg"};

C_SoundManager *C_SoundManager::singleton = NULL;

C_SoundManager::C_SoundManager()
{
   for(int i =0; i < SOUNDS; ++i) {
      m_SoundBuffers[i].loadFromFile(soundFiles[i]);
      m_Sounds[i].setBuffer(m_SoundBuffers[i]);
   }

   m_Sounds[SOUND_RUSH].setLoop(true);
   m_Sounds[SOUND_RUSH].setVolume(40);
   m_Sounds[SOUND_TURN].setVolume(20);
}

C_SoundManager *
C_SoundManager::GetSingleton(void)
{
   if(!singleton) {
      return new C_SoundManager();
   }

   return singleton;
}

void
C_SoundManager::PlaySound(unsigned int sound)
{
   if(sound < SOUNDS) {
      if(!m_Sounds[sound].getStatus()) {
         m_Sounds[sound].play();
      }
   }
}
