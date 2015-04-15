#include "DensityMap.h"
#include "RVO.h"
#include <cmath>


extern RVO::RVOSimulator *sim;


void DensityMap::VelFromDensity(std::function<float(float)> velFunc)
{
	for (size_t i = 0; i < sim->getNumAgents(); ++i)
	{
		int tempY = static_cast<int>(sim->getAgentPosition(i).y() / gridLength) - 1 + 240 / gridLength;
		int tempX = static_cast<int>(sim->getAgentPosition(i).x() / gridLength) - 1 + 320 / gridLength;
		sim->setAgentMaxSpeed(i, velFunc(DMap[tempY][tempX] / (gridLength*gridLength)));
	}
}

void DensityMap::CalcDensity()
{
#ifdef _OPENMP
#pragma omp parallel for
#endif
	//grid length is 10
	for (int j = 0; j < 480 / gridLength; ++j)
		for (int i = 0; i < 640 / gridLength; ++i)
		{
			DMap[j][i] = 0;
		}
	//static float weight = 1.0;
	//coordinate of center point is (0,0)
	for (size_t i = 0; i < sim->getNumAgents(); ++i) 
	{
		//if (sim->getAgentPosition(i).x() >= 320 || sim->getAgentPosition(i).y() >= 240 || sim->getAgentPosition(i).x() <= -320 || sim->getAgentPosition(i).y() <= -240)
			//continue;
		auto newVel = sim->getAgentPosition(i) + normalize(sim->getAgentVelocity(i))*8.0f;
		int tempY = static_cast<int>(newVel.y() / gridLength) - 1 + 240 / gridLength;
		int tempX = static_cast<int>(newVel.x() / gridLength) - 1 + 320 / gridLength;
		DMap[tempY][tempX] += 1;
		DMap[tempY - 1][tempX - 1] += 0.5;
		DMap[tempY - 1][tempX] += 0.5;
		DMap[tempY - 1][tempX + 1] += 0.5;
		DMap[tempY][tempX - 1] += 0.5;
		DMap[tempY][tempX + 1] += 0.5;
		DMap[tempY + 1][tempX - 1] += 0.5;
		DMap[tempY + 1][tempX] += 0.5;
		DMap[tempY + 1][tempX + 1] += 0.5;
	}
		
}

void DensityMap::seeForward()
{
	
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i)
	{
		auto newPos = sim->getAgentPosition(i) + normalize(sim->getAgentVelocity(i))*8.0f;
		float rotateAngle = atan2(sim->getAgentVelocity(i).y(), sim->getAgentVelocity(i).x()) + 90;
		RVO::Vector2 rotateVec = normalize(RVO::Vector2::Vector2(cos(rotateAngle), sin(rotateAngle)));
		int crowdCount = 0;
		if (sim->getTurnRight(i))
		{
			while (DMap[static_cast<int>(newPos.y() / gridLength) - 1 + 240 / gridLength][static_cast<int>(newPos.x() / gridLength) - 1 + 320 / gridLength] >= 5)
			{
				//rotate 45 degree to right 
				++crowdCount;
				newPos += rotateVec*8.0f;
				sim->setAgentPrefVelocity(i, normalize(newPos - sim->getAgentPosition(i)));
			}
			//if crowding, then rotate 45 degree to left
			if (crowdCount >= 2)
			{
				newPos -= rotateVec*8.0f*crowdCount;
				sim->setAgentPrefVelocity(i, normalize(newPos - sim->getAgentPosition(i)));
				sim->setTurnRight(i, false);
			}
		}
		else
		{
			while (DMap[static_cast<int>(newPos.y() / gridLength) - 1 + 240 / gridLength][static_cast<int>(newPos.x() / gridLength) - 1 + 320 / gridLength] >=5)
			{
				//rotate 45 degree to right 
				++crowdCount;
				newPos -= rotateVec*8.0f;
				sim->setAgentPrefVelocity(i, normalize(newPos - sim->getAgentPosition(i)));
			}
			//if crowding, then rotate 45 degree to left
			if (crowdCount >= 2)
			{
				newPos += rotateVec*8.0f*crowdCount;
				sim->setAgentPrefVelocity(i, normalize(newPos - sim->getAgentPosition(i)));
				sim->setTurnRight(i, true);
			}
		}
		
		
		
	}

}
