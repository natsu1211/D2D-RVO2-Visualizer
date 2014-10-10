/*Help class for visualization of density */

#ifndef _DENSITY_MAP_H_
#define _DENSITY_MAP_H_

class DensityMap
{
public:
	DensityMap()
	{
		for (int j = 0; j < 24; ++j)
			for (int i = 0; i < 32; ++i)
			{
				DMap[j][i] = 0;
			}
		maxDensity = 0;
		minDensity = 0;
	}
	void CalcDensity();
	void seeForward();
public:
	int DMap[24][32];
	int maxDensity;
	int minDensity;
};

#endif