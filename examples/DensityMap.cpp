#include "DensityMap.h"
#include "RVO.h"
#include <cmath>
#include <algorithm>    // std::max
#include "Vector2.h"
#include <fstream>


extern RVO::RVOSimulator *sim;
using namespace RVO;
std::ofstream fileOut2;

void DensityMap::VelFromDensity(std::function<float(float)> velFunc)
{
	for (size_t i = 0; i < sim->getNumAgents(); ++i)
	{
		int tempY = static_cast<int>(sim->getAgentPosition(i).y() / gridLength) - 1 + 240 / gridLength;
		int tempX = static_cast<int>(sim->getAgentPosition(i).x() / gridLength) - 1 + 320 / gridLength;
		sim->setAgentMaxSpeed(i, std::min(velFunc(DMap[tempY][tempX] / (gridLength*gridLength)), 1.4f));
	}
}

void DensityMap::CalcDensity()
{
#ifdef _OPENMP
#pragma omp parallel for
#endif

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
		//5s
		auto newVel = sim->getAgentPosition(i) + sim->getAgentVelocity(i)*5.0f;
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

	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i)
	{

		//float rotateAngle = atan2(sim->getAgentVelocity(i).y(), sim->getAgentVelocity(i).x()) + 90;
		//RVO::Vector2 rotateVec = normalize(RVO::Vector2::Vector2(cos(rotateAngle), sin(rotateAngle)));
		float minDensityFactor = 100000.f;
		int minDAng = 100000;
		Vector2 newPos1 = sim->getAgentPosition(i) + sim->getAgentVelocity(i)*5.0f;
		float newDensityFactor1 = (DMap[static_cast<int>(newPos1.y() / gridLength) - 1 + 240 / gridLength][static_cast<int>(newPos1.x() / gridLength) - 1 + 320 / gridLength]) / (gridLength*gridLength);
		if (newDensityFactor1 > 0.5)
		{
			for (double ang = -75; ang <= 75; ang += 15)
			{
				//posotion 5s later
				Vector2 newPos = sim->getAgentPosition(i) + Rotate(sim->getAgentVelocity(i), ang)*5.0f;
				float newDensityFactor = (DMap[static_cast<int>(newPos.y() / gridLength) - 1 + 240 / gridLength][static_cast<int>(newPos.x() / gridLength) - 1 + 320 / gridLength]) / (gridLength*gridLength) * (1 + std::abs(sin(ang / 180 * pi)));

				if (newDensityFactor < minDensityFactor)
				{
					minDensityFactor = newDensityFactor;
					minDAng = ang;
				}

			}
		}
		if (minDAng != 100000)
			sim->setAgentPrefVelocity(i, normalize(Rotate(sim->getAgentVelocity(i), minDAng)));
		//fileOut2.open("d://blockAng2.txt", std::ofstream::app);
		//fileOut2 << minDAng << " " << minDensityFactor << std::endl;
		//fileOut2.close();

	}

	
}

