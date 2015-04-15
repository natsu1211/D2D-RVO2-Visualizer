/*
 * ExampleCircle.cpp
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
 * 2014.05
 * Example file showing a demo with 100 agents initially positioned evenly
 * distributed on a circle attempting to move to the antipodal position on the
 * circle.
----------------------------------------------------------------------------- */

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>
#include <fstream>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "RVO.h"
#include "RVOCircleRender.h"
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
Clock clock;
DensityMap dmap;
std::ofstream fileOut;
const int numAgent = 300;
void setupScenario(RVO::RVOSimulator *sim)
{
	/* Specify the global time step of the simulation. */
	sim->setTimeStep(0.25f);

	/* Specify the default parameters for agents that are subsequently added. */
	sim->setAgentDefaults(15.0f, 10, 10.0f, 10.0f, 1.2f, 2.0f);

	/*
	 * Add agents, specifying their start position, and store their goals on the
	 * opposite side of the environment.
	 */

	
	for (size_t i = 0; i < numAgent; ++i) {
		sim->addAgent(150.0f *
			RVO::Vector2(std::cos(i * 2.0f * M_PI / numAgent),
			std::sin(i * 2.0f * M_PI / numAgent)));
		goals.push_back(-sim->getAgentPosition(i));
	}
	
	


}


void updateVisualization(RVO::RVOSimulator *sim)
{

	clock.GetTotalElapsedTime();
	clock.CalcTotalFPS();
	clock.RecordRenderCounter();
	clock.CalcRenderFPS();

	//fileOut.open("d://circle3000.txt", std::ofstream::app);
	//fileOut << clock.UpdateFPS() << std::endl;
	//fileOut.close();

	static WCHAR time[30];
	swprintf_s<30>(time, L"Simulation Time: %5.2f", clock.TotalRealTime());
	static WCHAR totalfps[30];
	swprintf_s<30>(totalfps, L"Total FPS: %3.0f", clock.TotalFPS());
	static WCHAR renderfps[30];
	swprintf_s<30>(renderfps, L"Render FPS: %3.0f", clock.RenderFPS());
	static WCHAR updatefps[30];
	swprintf_s<30>(updatefps, L"Update FPS: %3.0f", clock.UpdateFPS());

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
	
	//physical pixel to dip
	app.m_pCompatibleRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(wndWidth / 2 * 96.f / dpiX, wndHeight / 2 * 96.f / dpiY));
	static const float startColorRgb = 0.2f;//0.0 to 1.0
	static const float endColorRgb = 0.8f;//0.0 to 1.0
	static const float dColorRgb = (endColorRgb - startColorRgb) / static_cast<float>(numAgent);
	static ID2D1SolidColorBrush* brushArray[numAgent] = { nullptr };
	
	/* Output the current position of all the agents. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(
			D2D1::Point2F(sim->getAgentPosition(i).x() , sim->getAgentPosition(i).y() ),
			1.2f,
			1.2f
			);
		float rgb= startColorRgb + dColorRgb*i;
		app.m_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF::ColorF(0.9, 1-rgb, rgb),
			&brushArray[i]
			);
		app.m_pCompatibleRenderTarget->FillEllipse(ellipse, brushArray[i]/*app.m_pCornflowerBlueBrush*/);
	}
	/*visulize the density map*/
	/*
	ID2D1SolidColorBrush* densityBrush = nullptr;
	for (int j = 0; j < 24; ++j)
	{
		for (int i = 0; i < 32; ++i)
		{
			D2D1_RECT_F rect = D2D1::RectF(i * 20 - 320, j * 20 - 240, i * 20 - 320 + 20, j * 20 - 240 + 20);
			auto c = dmap.DMap[j][i] / 10.f >= 1 ? 1 : dmap.DMap[j][i] / 10.f;
			app.m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF::ColorF(c, 1.0-c, 0, 0.4),
				&densityBrush
				);
			app.m_pCompatibleRenderTarget->FillRectangle(rect, densityBrush);
			
		}
	}
	*/
	
		
	D2D1_POINT_2U pt{ 0, 0 };
	D2D1_RECT_U rct{ 0, 0, wndWidth, wndHeight };
	app.m_pBitmap->CopyFromRenderTarget(&pt, app.m_pCompatibleRenderTarget, &rct);
	app.m_pCompatibleRenderTarget->EndDraw();
	app.m_pRenderTarget->BeginDraw();
	app.m_pRenderTarget->DrawBitmap(app.m_pBitmap);
	app.m_pRenderTarget->EndDraw();
	
	clock.GetRenderElaspedTime();
	
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
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i) { //openmp needs an int 
		RVO::Vector2 goalVector = goals[i] - sim->getAgentPosition(i);

		if (RVO::absSq(goalVector) > 1.0f) {
			goalVector = RVO::normalize(goalVector);
		}

		sim->setAgentPrefVelocity(i, goalVector);
	}
}

bool reachedGoal(RVO::RVOSimulator *sim)
{
	/* Check if all agents have reached their goals. */
	for (size_t i = 0; i < sim->getNumAgents(); ++i) {
		if (RVO::absSq(sim->getAgentPosition(i) - goals[i]) > FLT_EPSILON/*RVO::absSq(sim->getAgentPosition(i) - goals[i]) > sim->getAgentRadius(i) * sim->getAgentRadius(i)*/) {
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
	// Use HeapSetInformation to specify that the process should
	// terminate if the heap manager detects an error in any heap used
	// by the process.
	// The return value is ignored, because we want to continue running in the
	// unlikely event that HeapSetInformation fails.
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
						clock.RecordUpdateCounter();
						clock.CalcUpdateFPS();
						setPreferredVelocities(sim);
						dmap.seeForward();
						sim->doStep(); 
						dmap.CalcDensity();
						dmap.VelFromDensity(std::bind(&DensityMap::modelFunc2, &dmap, _1));
						clock.GetUpdateElaspedTime();
						updateVisualization(sim);
						
					}
					
				}
				
			}
		
		CoUninitialize();
	}
	//fileOut.open("d://circle2000d4.txt", std::ofstream::app);
	//fileOut << clock.TotalRealTime() << std::endl;
	//fileOut.close();
	delete sim;
	return 0;
}
