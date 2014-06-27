#include "Clock.h"

void Clock::RecordUpdateCounter()
{
	// get time from Windows
	LONGLONG frequency;
	bool clockOK = (QueryPerformanceCounter((LARGE_INTEGER *)&updateBaseCounter) &&
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequency));
	
}

void Clock::RecordRenderCounter()
{
	// get time from Windows
	LONGLONG frequency;
	bool clockOK = (QueryPerformanceCounter((LARGE_INTEGER *)&renderBaseCounter) &&
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequency));
}

float Clock::GetUpdateElaspedTime()
{
	// get time from Windows
	LONGLONG counter, frequency;
	bool clockOK = (QueryPerformanceCounter((LARGE_INTEGER *)&counter) &&
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequency));
	if (!clockOK)
		return 0.0f;

	// real "wall clock" time since launch
	const LONGLONG counterDifference = counter - updateBaseCounter;
	elapsedUpdateTime = ((float)counterDifference) / ((float)frequency);
	AccTotalUpdateTime();
	return elapsedUpdateTime;
}

float Clock::GetRenderElaspedTime()
{
	// get time from Windows
	LONGLONG counter, frequency;
	bool clockOK = (QueryPerformanceCounter((LARGE_INTEGER *)&counter) &&
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequency));
	if (!clockOK)
		return 0.0f;

	// real "wall clock" time since launch
	const LONGLONG counterDifference = counter - renderBaseCounter;
	elapsedRenderTime = ((float)counterDifference) / ((float)frequency);
	AccTotalRenderTime();
	return elapsedUpdateTime;
}

float Clock::GetTotalElapsedTime()
{
	// get time from Windows
	LONGLONG counter, frequency;
	bool clockOK = (QueryPerformanceCounter((LARGE_INTEGER *)&counter) &&
		QueryPerformanceFrequency((LARGE_INTEGER *)&frequency));
	if (!clockOK)
		return 0.0f;
	// ensure the base counter is recorded once after launch
	if (totalBaseCounter == 0) totalBaseCounter = counter;
	// real "wall clock" time since launch
	const LONGLONG counterDifference = counter - totalBaseCounter;
	totalBaseCounter = counter;
	elapsedTotalTime = ((float)counterDifference) / ((float)frequency);
	AccTotalRealTime();
	return elapsedTotalTime;
}
/*
void Clock::CalcTotalFPS()
{ 
	static int frameCount = 0;
	static float elapsedTime = totalRealTime;
	++frameCount;//plus 1 everytime been called

	//In order to get smoother FPS, we calculate FPS at every 0.1s, instead of every frame
	if (totalRealTime - elapsedTime > 0.1f)
	{
		totalFPS = static_cast<float>(frameCount) / (totalRealTime - elapsedTime) ;
		elapsedTime = totalRealTime;
		frameCount = 0;
	}
}
void Clock::CalcUpdateFPS()
{ 
	static int frameCount1 = 0;
	static float elapsedTime1 = totalUpdateTime;
	++frameCount1;//plus 1 everytime been called

	//In order to get smoother FPS, we calculate FPS at every 0.002s, instead of every frame
	if (totalUpdateTime - elapsedTime1 > 0.002f)
	{
		updateFPS = static_cast<float>(frameCount1) / (totalUpdateTime - elapsedTime1);
		elapsedTime1 = totalUpdateTime;
		frameCount1 = 0;
	}
}
void Clock::CalcRenderFPS()
{ 
	static int frameCount2 = 0;
	static float elapsedTime2 = totalRenderTime;
	++frameCount2;//plus 1 everytime been called

	//In order to get smoother FPS, we calculate FPS at every 0.1s, instead of every frame
	if (totalRenderTime - elapsedTime2 > 0.1f)
	{
		renderFPS = static_cast<float>(frameCount2) / (totalRenderTime - elapsedTime2);
		elapsedTime2 = totalRenderTime;
		frameCount2 = 0;
	}
}
*/