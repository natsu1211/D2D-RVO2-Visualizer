/*
 * ExampleBlocks.cpp
 * RVO2 Library
 *
 * Copyright (c) 2008-2010 University of North Carolina at Chapel Hill.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for educational, research, and non-profit purposes, without
 * fee, and without a written agreement is hereby granted, provided that the
 * above copyright notice, this paragraph, and the following four paragraphs
 * appear in all copies.
 *
 * Permission to incorporate this software into commercial products may be
 * obtained by contacting the Office of Technology Development at the University
 * of North Carolina at Chapel Hill <otd@unc.edu>.
 *
 * This software program and documentation are copyrighted by the University of
 * North Carolina at Chapel Hill. The software program and documentation are
 * supplied "as is," without any accompanying services from the University of
 * North Carolina at Chapel Hill or the authors. The University of North
 * Carolina at Chapel Hill and the authors do not warrant that the operation of
 * the program will be uninterrupted or error-free. The end-user understands
 * that the program was developed for research purposes and is advised not to
 * rely exclusively on the program for any reason.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL OR THE
 * AUTHORS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS
 * SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF NORTH CAROLINA AT
 * CHAPEL HILL OR THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE AUTHORS SPECIFICALLY
 * DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE AND ANY
 * STATUTORY WARRANTY OF NON-INFRINGEMENT. THE SOFTWARE PROVIDED HEREUNDER IS ON
 * AN "AS IS" BASIS, AND THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL AND THE
 * AUTHORS HAVE NO OBLIGATIONS TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Please send all bug reports to <geom@cs.unc.edu>.
 *
 * The authors may be contacted via:
 *
 * Jur van den Berg, Stephen J. Guy, Jamie Snape, Ming C. Lin, Dinesh Manocha
 * Dept. of Computer Science
 * 201 S. Columbia St.
 * Frederick P. Brooks, Jr. Computer Science Bldg.
 * Chapel Hill, N.C. 27599-3175
 * United States of America
 *
 * <http://gamma.cs.unc.edu/RVO2/>
 */

/*-----------------------------------------------------------------------------
* modified by long
* 2014.06
* Example file showing a demo with 100 agents split in four groups initially
* positioned in four corners of the environment. Each agent attempts to move to
* other side of the environment through a narrow passage generated by four
* obstacles. There is no roadmap to guide the agents around the obstacles.
----------------------------------------------------------------------------- */

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <fstream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "RVO.h"
#include "RVOBlocksRender.h"
#include "Clock.h"
#include "DensityMap.h"

#ifndef M_PI
const float M_PI = 3.14159265358979323846f;
#endif

/* Store the goals of the agents. */
std::vector<RVO::Vector2> goals;
RVORender app;
/* Create a new simulator instance. */
RVO::RVOSimulator *sim = new RVO::RVOSimulator();
Clock clock2;
DensityMap dmap;
std::ofstream fileOut;
long frameCount = 0;
void setupScenario(RVO::RVOSimulator *sim)
{
	/* Seed the random number generator. */
	std::srand(static_cast<unsigned int>(std::time(0)));

	/* Specify the global time step of the simulation. */
	sim->setTimeStep(0.25f);

	/* Specify the default parameters for agents that are subsequently added. */
	sim->setAgentDefaults(15.0f, 5, 10.0f, 10.0f, 1.2f, 1.4f);

	/*
	 * Add agents, specifying their start position, and store their goals on the
	 * opposite side of the environment.
	 */
	
	/*
	for (size_t i = 0; i < 20; ++i) {
		for (size_t j = 0; j < 15; ++j) {
			sim->addAgent(RVO::Vector2(100.0f + i * 3.0f, 22.5f - j * 3.0f));
			goals.push_back(RVO::Vector2(-130.0f, 0.0f));

			sim->addAgent(RVO::Vector2(22.5f - j * 3.0f, 100.0f + i * 3.0f));
			goals.push_back(RVO::Vector2(0.0f, -130.0f));
		}
	}
	*/
	
	for (size_t i = 0; i < 20; ++i) {
		for (size_t j = 0; j < 25; ++j) {
			sim->addAgent(RVO::Vector2(90.0f + i * 3.0f, 30.0f - j * 3.0f));
			goals.push_back(RVO::Vector2(-120.0f, 0.0f));
			
			sim->addAgent(RVO::Vector2(30.0f - j * 3.0f , - 90.0f - i * 3.0f));
			goals.push_back(RVO::Vector2(0.0f, 120.0f));
			
			sim->addAgent(RVO::Vector2(-90.0f - i * 3.0f, 30.0f - j * 3.0f));
			goals.push_back(RVO::Vector2(120.0f, 0.0f));

			sim->addAgent(RVO::Vector2(30.0f - j * 3.0f, 90.0f + i * 3.0f));
			goals.push_back(RVO::Vector2(0.0f, -120.0f));
			

		}
	}
	
	/*
	 * Add (polygonal) obstacles, specifying their vertices in counterclockwise
	 * order.
	 */

	
	std::vector<RVO::Vector2> obstacle1, obstacle2, obstacle3, obstacle4;
	
	
	//left-lower
	
	obstacle1.push_back(RVO::Vector2(-30.0f, 240.0f));
	obstacle1.push_back(RVO::Vector2(-320.0f, 240.0f));
	obstacle1.push_back(RVO::Vector2(-320.0f, 30.0f));
	obstacle1.push_back(RVO::Vector2(-30.0f, 30.0f));
	
	//right-lower
	
	obstacle2.push_back(RVO::Vector2(30.0f, 240.0f));
	obstacle2.push_back(RVO::Vector2(30.0f, 30.0f));
	obstacle2.push_back(RVO::Vector2(320.0f, 30.0f));
	obstacle2.push_back(RVO::Vector2(320.0f, 240.0f));

	//right-upper
	obstacle3.push_back(RVO::Vector2(30.0f, -240.0f));
	obstacle3.push_back(RVO::Vector2(320.0f, -240.0f));
	obstacle3.push_back(RVO::Vector2(320.0f, -30.0f));
	obstacle3.push_back(RVO::Vector2(30.0f, -30.0f));

	//left-upper
	obstacle4.push_back(RVO::Vector2(-30.0f, -240.0f));
	obstacle4.push_back(RVO::Vector2(-30.0f, -30.0f));
	obstacle4.push_back(RVO::Vector2(-320.0f, -30.0f));
	obstacle4.push_back(RVO::Vector2(-320.0f, -240.0f));


	
	
	

	//sim->addObstacle(obstacle1);
	//sim->addObstacle(obstacle2);
	//sim->addObstacle(obstacle3);
	//sim->addObstacle(obstacle4);
	
	/* Process the obstacles so that they are accounted for in the simulation. */
	sim->processObstacles();
}

void updateVisualization(RVO::RVOSimulator *sim)
{
	clock2.GetTotalElapsedTime();
	clock2.CalcTotalFPS();
	clock2.RecordRenderCounter();
	clock2.CalcRenderFPS();

	fileOut.open("d://result721//cross-2d.txt", std::ofstream::app);
	fileOut << clock2.UpdateFPS() << std::endl;
	fileOut.close();

	static WCHAR time[40];
	swprintf_s<40>(time, L"Wallclock Time: %5.2f", clock2.TotalRealTime());
	static WCHAR totalfps[40];
	swprintf_s<40>(totalfps, L"Total FPS: %3.0f", clock2.TotalFPS());
	static WCHAR renderfps[40];
	swprintf_s<40>(renderfps, L"Render FPS: %3.0f", clock2.RenderFPS());
	static WCHAR updatefps[40];
	swprintf_s<40>(updatefps, L"Update FPS: %3.0f", clock2.UpdateFPS());
	static WCHAR globalTime[40];
	swprintf_s<40>(globalTime, L"Simulation Time: %5.2f", sim->getGlobalTime());

	static FLOAT dpiX, dpiY;
	app.m_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	app.m_pCompatibleRenderTarget->BeginDraw();
	app.m_pCompatibleRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
	app.m_pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	/*
	for (int x = 0; x < wndWidth; x += 30)
	{
		app.m_pCompatibleRenderTarget->DrawLine(
			D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2F(static_cast<FLOAT>(x), wndHeight),
			app.m_pLightSlateGrayBrush,
			1.0f
			);
	}

	for (int y = 0; y < wndHeight; y += 30)
	{
		app.m_pCompatibleRenderTarget->DrawLine(
			D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2F(wndWidth, static_cast<FLOAT>(y)),
			app.m_pLightSlateGrayBrush,
			1.0f
			);
	}
	*/
	
	app.m_pCompatibleRenderTarget->DrawText(
		time,
		ARRAYSIZE(time) - 1,
		app.m_pTextFormat,
		D2D1::RectF(0, 0, wndWidth, wndHeight),
		app.m_pLightSlateGrayBrush
		);
	app.m_pCompatibleRenderTarget->DrawText(
		totalfps,
		ARRAYSIZE(totalfps) - 1,
		app.m_pTextFormat,
		D2D1::RectF(0, 20, wndWidth, wndHeight),
		app.m_pLightSlateGrayBrush
		);
	app.m_pCompatibleRenderTarget->DrawText(
		renderfps,
		ARRAYSIZE(renderfps) - 1,
		app.m_pTextFormat,
		D2D1::RectF(0, 40, wndWidth, wndHeight),
		app.m_pLightSlateGrayBrush
		);
	app.m_pCompatibleRenderTarget->DrawText(
		updatefps,
		ARRAYSIZE(updatefps) - 1,
		app.m_pTextFormat,
		D2D1::RectF(0, 60, wndWidth, wndHeight),
		app.m_pLightSlateGrayBrush
		);
	app.m_pCompatibleRenderTarget->DrawText(
		globalTime,
		ARRAYSIZE(globalTime) - 1,
		app.m_pTextFormat,
		D2D1::RectF(0, 80, wndWidth, wndHeight),
		app.m_pLightSlateGrayBrush
		);
	
	//physical pixel to dip
	app.m_pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(wndWidth / 2 * 96.f / dpiX, wndHeight / 2 * 96.f / dpiY));

	static ID2D1SolidColorBrush* brushArray[4] = { nullptr };
	app.m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Blue),
		&brushArray[0]
		);
	app.m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Red),
		&brushArray[1]
		);
	
	app.m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Green),
		&brushArray[2]
		);
	app.m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Purple),
		&brushArray[3]
		);
	


	/* Output the current position of all the agents. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(
			D2D1::Point2F(sim->getAgentPosition(i).x(), sim->getAgentPosition(i).y()),
			1.2f,
			1.2f
			);
		/*
		if (0 == i % 2)
		    app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[0]);
		else if (1 == i % 2)
			app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[1]);
		*/
		if (0 == i % 4)
			app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[0]);
		else if (1 == i % 4)
			app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[1]);
		else if (2 == i % 4)
			app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[2]);
		else if (3 == i % 4)
			app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[3]);
		
	}

	//draw obstacles 
	/*
	static ID2D1SolidColorBrush* pBlackBrush = nullptr ;
	app.m_pRenderTarget->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		&pBlackBrush
		);
	//obstacle1
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(-320, -30),
		D2D1::Point2F(-30, -30),
		pBlackBrush,
		1.0f
		);
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(-30, -30),
		D2D1::Point2F(-30, -240),
		pBlackBrush,
		1.0f
		);
	//obstacle2
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(320, -30),
		D2D1::Point2F(30, -30),
		pBlackBrush,
		1.0f
		);
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(30, -30),
		D2D1::Point2F(30, -240),
		pBlackBrush,
		1.0f
		);
	//obstacle3
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(-320, 30),
		D2D1::Point2F(-30, 30),
		pBlackBrush,
		1.0f
		);
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(-30, 30),
		D2D1::Point2F(-30, 240),
		pBlackBrush,
		1.0f
		);
	//obstacle4
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(320, 30),
		D2D1::Point2F(30, 30),
		pBlackBrush,
		1.0f
		);
	app.m_pCompatibleRenderTarget->DrawLine(
		D2D1::Point2F(30, 30),
		D2D1::Point2F(30, 240),
		pBlackBrush,
		1.0f
		);
		*/
	
	//copy off-screen buffer to on-screen buffer
	D2D1_POINT_2U pt{ 0, 0 };
	D2D1_RECT_U rct{ 0, 0, wndWidth, wndHeight };
	app.m_pBitmap->CopyFromRenderTarget(&pt, app.m_pCompatibleRenderTarget, &rct);
	app.m_pCompatibleRenderTarget->EndDraw();
	app.m_pRenderTarget->BeginDraw();
	app.m_pRenderTarget->DrawBitmap(app.m_pBitmap);
	app.m_pRenderTarget->EndDraw();

	clock2.GetRenderElaspedTime();
}

void setPreferredVelocities(RVO::RVOSimulator *sim)
{
	/*
	 * Set the preferred velocity to be a vector of unit magnitude (speed) in the
	 * direction of the goal.
	 */
#ifdef _OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i) {
		RVO::Vector2 goalVector = goals[i] - sim->getAgentPosition(i);

		if (RVO::absSq(goalVector) > 1.0f) {
			goalVector = RVO::normalize(goalVector) * sim->getAgentMaxSpeed(i);
		}

		sim->setAgentPrefVelocity(i, goalVector);

		/*
		 * Perturb a little to avoid deadlocks due to perfect symmetry.
		 */
		float angle = std::rand() * 2.0f * M_PI / RAND_MAX;
		float dist = std::rand() * 0.0001f / RAND_MAX;

		sim->setAgentPrefVelocity(i, sim->getAgentPrefVelocity(i) +
		                          dist * RVO::Vector2(std::cos(angle), std::sin(angle)));
	}
}

bool reachedGoal(RVO::RVOSimulator *sim)
{
	/* Check if all agents have reached their goals. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		if (RVO::absSq(sim->getAgentPosition(i) - goals[i]) > 80.0f * 80.0f) {
			return false;
		}
	}

	return true;
}

// Provides the application entry point.
int WINAPI WinMain(
	HINSTANCE /* hInstance */,
	HINSTANCE /* hPrevInstance */,
	LPSTR /* lpCmdLine */,
	int /* nCmdShow */
	)
{
	using namespace std::placeholders;
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);
	/* Set up the scenario. */
	setupScenario(sim);

	if (SUCCEEDED(CoInitialize(NULL)))
	{
		if (SUCCEEDED(app.Initialize()))
		{
			MSG msg;
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else if (!reachedGoal(sim))
				{
					clock2.RecordUpdateCounter();
					clock2.CalcUpdateFPS();
					setPreferredVelocities(sim);
					dmap.seeForward();
					sim->doStep();
					dmap.CalcDensity();
					dmap.VelFromDensity(std::bind(&DensityMap::modelFunc1, &dmap, _1));
					clock2.GetUpdateElaspedTime();
					++frameCount;
					updateVisualization(sim);
					
				}
			}
		}
		CoUninitialize();
	}
	//fileOut.open("d://test.txt", std::ofstream::app);
	//fileOut << frameCount << std::endl;
	//fileOut.close();
	delete sim;
	return 0;
}
