#include "timer.h"

void C_Timer::Initialize(void)
{
	d0 = d1 = delta = 0.0f;
	freq = 1000.0f;
	timeFactor = 1.0f / freq;
	gettimeofday(&mmTimeStart, NULL);
}

float C_Timer::GetTime(void)
{
	static timeval currentTime;
	static float elapsedTime;

	gettimeofday(&currentTime, NULL);

	elapsedTime = (currentTime.tv_sec - mmTimeStart.tv_sec) * freq;      // sec to ms
    elapsedTime += (currentTime.tv_usec - mmTimeStart.tv_usec) * timeFactor;   // us to ms

	return elapsedTime;
}

void C_Timer::Update(void)
{
	d1 = GetTime();
	delta = d1 - d0;
	d0 = d1;
}
