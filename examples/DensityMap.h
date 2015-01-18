/*Help class for visualization of density */

#ifndef _DENSITY_MAP_H_
#define _DENSITY_MAP_H_
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
	void seeForward();
public:
	float DMap[480 / gridLength][640 / gridLength];
	int maxDensity;
	int minDensity;
};

#endif