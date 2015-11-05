//Created by long
//2014.06
//simple clock system for simulation


#ifndef _RVO_CLOCK_H_
#define _RVO_CLOCK_H_
#include "windows.h"

namespace LL
{
	class Clock
	{
	public:
		Clock() :totalRealTime(0), totalUpdateTime(0), totalRenderTime(0),
			elapsedTotalTime(0), elapsedUpdateTime(0), elapsedRenderTime(0),
			totalBaseCounter(0), updateBaseCounter(0), renderBaseCounter(0),
			totalFPS(0), updateFPS(0), renderFPS(0)
		{};
		inline float TotalRealTime() { return totalRealTime; }
		inline float TotalUpdateTime() { return totalUpdateTime; }
		inline float TotalRenderTime() { return totalRenderTime; }
		//inline void SetTotalRealTime(float time){ totalRealTime = time; }
		//inline void SetTotalUpdateTime(float time){ totalUpdateTime = time; }
		//inline void SetTotalRenderTime(float time){ totalRenderTime = time; }
		inline float TotalFPS() { return totalFPS; }
		inline float UpdateFPS() { return updateFPS; }
		inline float RenderFPS() { return renderFPS; }
		inline void CalcTotalFPS() { totalFPS = 1 / elapsedTotalTime; }
		inline void CalcUpdateFPS() { updateFPS = 1 / elapsedUpdateTime; }
		inline void CalcRenderFPS() { renderFPS = 1 / elapsedRenderTime; }
		//call this function at the veryfirst of update phase, and call GetUpdateElaspedTime() when update phase ended.
		void RecordUpdateCounter();
		//call this function at the veryfirst of render phase, and call GetUpdateElaspedTime() when render phase ended.
		void RecordRenderCounter();
		float GetTotalElapsedTime();
		float GetUpdateElaspedTime();
		float GetRenderElaspedTime();




	private:
		// real "wall clock" time since launch
		float totalRealTime;
		// total time the update part has run
		float totalUpdateTime;
		// total time the update part has run
		float totalRenderTime;


		// interval since last simulation time step
		float elapsedTotalTime;
		// interval since last update time step
		float elapsedUpdateTime;
		// interval since last update time step
		float elapsedRenderTime;

		LONGLONG totalBaseCounter;
		LONGLONG updateBaseCounter;
		LONGLONG renderBaseCounter;

		float totalFPS;
		float updateFPS;
		float renderFPS;

		inline void AccTotalRealTime() { totalRealTime += elapsedTotalTime; }
		inline void AccTotalUpdateTime() { totalUpdateTime += elapsedUpdateTime; }
		inline void AccTotalRenderTime() { totalRenderTime += elapsedRenderTime; }
	};

}

#endif //_RVO_CLOCK_H_
