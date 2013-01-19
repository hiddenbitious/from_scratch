/****************************************
*     ***************************       *
*         Diplomatiki Ergasia:			*
*                                       *
*		  Meleti kai Ylopoiish			*
*		  Algorithmon Grafikon			*
*                                       *
*     ***************************       *
*                                       *
*			  Syggrafeas:               *
*                                       *
*		  Apostolou Panagiotis			*
*                                       *
*     ***************************       *
****************************************/

#include "timer.h"

C_Timer::C_Timer ()
{
	d0 = 0;
	d1 = 0;
	delta = 0;
	perfFlag = false;
}


void C_Timer::Initialize ( void )
{
	//Check if the machine supports high-resolution performance counter
	if ( QueryPerformanceFrequency ( (LARGE_INTEGER *) &freq ) )
	{
		perfFlag = true;
		timeFactor = 1.0f / (float)freq;
		QueryPerformanceCounter ( (LARGE_INTEGER *) &perfTimeStart );
	}
	else
	{
		freq = 1000.0f;
		perfFlag = false;
		timeFactor = 1.0f / freq;
		mmTimeStart = timeGetTime ();
	}
}


ULONG C_Timer::GetTime ( void )
{
	if ( perfFlag )
	{
		__int64 time;
		QueryPerformanceCounter ( ( LARGE_INTEGER *) &time );
		return ( (time - perfTimeStart) * timeFactor ) * 1000.0f;
	}
	else
		return ( (timeGetTime () - mmTimeStart) * timeFactor ) * 1000.0f;
}


void C_Timer::Update ( void )
{
	d1 = this->GetTime ();
	delta = d1-d0;
	d0 = d1;
}
