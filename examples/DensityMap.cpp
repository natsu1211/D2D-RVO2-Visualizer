#include "DensityMap.h"
#include "RVO.h"
#include <cmath>

extern RVO::RVOSimulator *sim;
const int gridLength = 20;

// look forword for 3 seconds
void DensityMap::CalcDensity()
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	//grid length is 20
	for (int j = 0; j < 24; ++j)
		for (int i = 0; i < 32; ++i)
		{
			DMap[j][i] = 0;
		}
	//coordinate of center point is (0,0)
	for (size_t i = 0; i < sim->getNumAgents(); ++i) 
	{
		auto newVel = sim->getAgentPosition(i) + sim->getAgentVelocity(i)*12.0f;
		DMap[static_cast<int>(newVel.y() / gridLength) - 1 + 12][static_cast<int>(newVel.x() / gridLength) - 1 + 16] += 1;
	}
		
}

void DensityMap::seeForward()
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i)
	{
		auto newPos = sim->getAgentPosition(i) + sim->getAgentVelocity(i)*12.0f;
		float rotateAngle = atan2(sim->getAgentVelocity(i).y(), sim->getAgentVelocity(i).x()) + 90;
		RVO::Vector2 rotateVec = normalize(RVO::Vector2::Vector2(cos(rotateAngle), sin(rotateAngle)));
		while (DMap[static_cast<int>(newPos.y() / gridLength) - 1 + 12][static_cast<int>(newPos.x() / gridLength) - 1 + 16] >= 10)
		{
			newPos += rotateVec*10.0f;
			sim->setAgentPrefVelocity(i, normalize(newPos - sim->getAgentPosition(i)));
		}
		
	}

}
