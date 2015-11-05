/*Help class for visualization of density */

#ifndef _DENSITY_MAP_H_
#define _DENSITY_MAP_H_

#include <functional>
#include <cmath>
const int gridLength = 5;

class DensityMap
{
public:
	DensityMap()
	{
		for (int j = 0; j < 480 / gridLength; ++j)
			for (int i = 0; i < 640 / gridLength; ++i)
			{
				DMap[j][i] = 0;
			}
		maxDensity = 0;
		minDensity = 0;
	}
	void CalcDensity();
	void VelFromDensity(std::function<float(float)>);
	void seeForward();
public:
	float DMap[480 / gridLength][640 / gridLength];
	int maxDensity;
	int minDensity;
	
public:
	#undef min
	float modelFunc1(float density)
	{
		return 1.272*pow(density, -0.7954);
	}
	
	float modelFunc2(float density)
	{
		return 1.5 / density;
	}

	float modelFunc3(float density)
	{
		return 1.48 - 0.28*density;
	}

	float modelFunc4(float density)
	{
		return -0.26 + sqrt(2.4 / (density - 0.13) > 0.0f?2.4 / (density - 0.13):0.0f);
	}

	float modelFunc5(float density)
	{
		return 1.365 - 0.341*density;
	}
};

#endif