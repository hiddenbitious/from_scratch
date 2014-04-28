#ifndef _C_TIMER_H_
#define _C_TIMER_H_

#include <sys/time.h>
#include "globals.h"

/*
3 different ways to keep time:

DWORD timeGetTime () :
//	This function provides a rock steady 1 millisecond time base, and is sufficient for timing applications that
//	do not require a resolution of below a millisecond.

//	Note, however, that it also has the longest execution time of any of these functions,
//	so it should not be used for polling in a tight loop.

DWORD GetTickCount () :
//	The accuracy of this function is very much dependent upon the operating system it is ran on,
//	and if too coarse to be relied upon for frame timing.

//	On the other hand, the low overhead of calling this function makes it ideal polling,
//	and is a good candidate for timing low priority background tasks.

BOOL QueryPerformanceFrequency (LARGE_INTEGER *lpFrequency)
//	If accuracy and resolution are key, as is the case in frame timing, the Performance Counter
//	is hands down the way to go.
//	This function is not available on all machines, so your application should test for its presence before using it.

This class uses the performance counter and if it's not available uses the multimedia timer (timeGetTime) instead.
*/

class C_Timer
{
   private:
      float timeFactor;          //To convert the time received
      int64_t freq;              //Timer frequency

      float delta, d0, d1;       //Used to calculate delta
      timeval mmTimeStart;       //Multimedia timer start value

   public:
      C_Timer(void) {}           //CTor
      void Initialize(void);     //Initializes timer
      float GetTime(void);       //Returns since the timer initialization

      void Update(void);         //Updates the delta variable

      /// Returns the delta variable
      inline float GetDelta(void) { return delta; }
};

#endif
