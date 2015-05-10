#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <SFML/Audio.hpp>

#define SOUNDS  4

#define SOUND_INTRO     0
#define SOUND_STEP      1
#define SOUND_RUSH      2
#define SOUND_TURN      3

class C_SoundManager {

private:
   sf::SoundBuffer m_SoundBuffers[SOUNDS];
   sf::Sound m_Sounds[SOUNDS];

   C_SoundManager();

   static C_SoundManager *sinlgeton;

public:
   static C_SoundManager *GetSingleton(void);

   void PlaySound(unsigned int sound);
};

#endif // __AUDIO_H__
