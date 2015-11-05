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
#include <boost/polygon/voronoi.hpp>
#include <boost/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/strategies/strategies.hpp>
#include <boost/geometry/algorithms/make.hpp>
#include <boost/geometry/io/dsv/write.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/algorithms/within.hpp>

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

using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;
using namespace boost::polygon;


/* Store the goals of the agents. */
std::vector<RVO::Vector2> goals;
RVORender app;
/* Create a new simulator instance. */
RVO::RVOSimulator *sim = new RVO::RVOSimulator();
LL::Clock simClock;
DensityMap dmap;
std::ofstream fileOut;
const int numAgent = 300;
void setupScenario(RVO::RVOSimulator *sim)
{
	/* Specify the global time step of the simulation. */
	sim->setTimeStep(0.25f);

	/* Specify the default parameters for agents that are subsequently added. */
	sim->setAgentDefaults(8.0f, 5, 10.0f, 10.0f, 1.2f, 1.4f);

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

namespace boost {
	namespace polygon {
		template <>
		struct geometry_concept<RVO::Vector2> { typedef point_concept type; };

		template <>
		struct point_traits<RVO::Vector2> {
			typedef float coordinate_type;

			static inline coordinate_type get(const RVO::Vector2& point, orientation_2d orient) {
				return (orient == HORIZONTAL) ? point.x() : point.y();
			}
		};

		template <>
		struct geometry_concept<RVO::Segment> {
			typedef segment_concept type;
		};

		template <>
		struct segment_traits<RVO::Segment> {
			typedef float coordinate_type;
			typedef RVO::Vector2 point_type;

			static inline point_type get(const RVO::Segment& segment, direction_1d dir) {
				return dir.to_int() ? segment.end : segment.start;
			}
		};
	}
}

typedef double coordinate_type;
typedef point_data<coordinate_type> point_type;
typedef rectangle_data<coordinate_type> rect_type;
typedef voronoi_builder<int> VB;
typedef voronoi_diagram<coordinate_type> VD;
typedef VD::cell_type cell_type;
typedef VD::cell_type::source_index_type source_index_type;
typedef VD::cell_type::source_category_type source_category_type;
typedef VD::edge_type edge_type;
typedef VD::cell_container_type cell_container_type;
typedef VD::cell_container_type vertex_container_type;
typedef VD::edge_container_type edge_container_type;
typedef VD::const_cell_iterator const_cell_iterator;
typedef VD::const_vertex_iterator const_vertex_iterator;
typedef VD::const_edge_iterator const_edge_iterator;

typedef boost::geometry::model::d2::point_xy<double> point_2d;
typedef boost::geometry::model::polygon<point_2d> polygon_2d;


BOOST_GEOMETRY_REGISTER_POINT_2D(RVO::Vector2, float, cs::cartesian, x_, y_)


std::vector<polygon_2d> polygons(numAgent);
std::vector<double> voronoiDensity(numAgent);
void seeForwardVoronoi(voronoi_diagram<double> &vd)
{
	
	for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it)
	{
		using namespace boost::geometry;
		if (it->contains_point())
		{
			std::size_t index = it->source_index();
			const voronoi_diagram<double>::cell_type &cell = *it;
			const voronoi_diagram<double>::edge_type *edge = cell.incident_edge();
			polygon_2d poly;
			do {
				if (edge->is_primary())
				{
					append(poly.outer(), make<point_2d>(edge->vertex1()->x(), edge->vertex1()->y()));
				}
				edge = edge->next();
			} while (edge != cell.incident_edge());

			
			//correct(poly);
			if (index < numAgent)
			{
				polygons[index] = poly;
				voronoiDensity[index] = std::abs(1 / area(poly));

			}
				
		}
	}
	for (int i = 0; i < static_cast<int>(sim->getNumAgents()); ++i)
	{

		//float rotateAngle = atan2(sim->getAgentVelocity(i).y(), sim->getAgentVelocity(i).x()) + 90;
		//RVO::Vector2 rotateVec = normalize(RVO::Vector2::Vector2(cos(rotateAngle), sin(rotateAngle)));
		using namespace RVO;
		Vector2 newPos1 = sim->getAgentPosition(i) + sim->getAgentVelocity(i)*5.0f;
		int cellIndex = -1;
		for (int j = 0; j < polygons.size();++j)
		{
			if (boost::geometry::within(point_2d(newPos1.x(), newPos1.y()), polygons[j]))
			{
				cellIndex = j;
			}
		}
		assert(cellIndex != -1);
		if (cellIndex >= numAgent)
			return;
		
		float minDensityFactor = 100000.f;
		int minDAng = 100000;
		
		//see the new pos is in which cell

		
		
		float newDensityFactor1 = voronoiDensity[cellIndex];
		if (newDensityFactor1 > 0.5)
		{
			for (double ang = -75; ang <= 75; ang += 15)
			{
				//posotion 5s later
				Vector2 newPos = sim->getAgentPosition(i) + Rotate(sim->getAgentVelocity(i), ang)*5.0f;

				float newDensityFactor = voronoiDensity[cellIndex] / cos(ang / 180 * pi);

				if (newDensityFactor < minDensityFactor)
				{
					minDensityFactor = newDensityFactor;
					minDAng = ang;
				}

			}
		}
		if (minDAng != 100000)
			sim->setAgentPrefVelocity(i, normalize(Rotate(sim->getAgentVelocity(i), minDAng)));

	}


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

ID2D1SolidColorBrush* voronoiBrush = nullptr;

void updateVisualization(RVO::RVOSimulator *sim)
{
	voronoi_diagram<double> vd;
	simClock.GetTotalElapsedTime();
	simClock.CalcTotalFPS();
	simClock.RecordRenderCounter();
	simClock.CalcRenderFPS();

	//fileOut.open("d://circle3000.txt", std::ofstream::app);
	//fileOut << simClock.UpdateFPS() << std::endl;
	//fileOut.close();

	static WCHAR time[30];
	swprintf_s<30>(time, L"WallClock Time: %5.2f", simClock.TotalRealTime());
	static WCHAR totalfps[30];
	swprintf_s<30>(totalfps, L"Total FPS: %3.0f", simClock.TotalFPS());
	static WCHAR renderfps[30];
	swprintf_s<30>(renderfps, L"Render FPS: %3.0f", simClock.RenderFPS());
	static WCHAR updatefps[30];
	swprintf_s<30>(updatefps, L"Update FPS: %3.0f", simClock.UpdateFPS());
	static WCHAR globalTime[30];
	swprintf_s<30>(globalTime, L"Simulation Time: %5.2f", sim->getGlobalTime());

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

	//visulize the voronoi diagram
	// Construction of the Voronoi Diagram.
	
	std::vector<RVO::Vector2> points;
	for (size_t i = 0; i < sim->getNumAgents(); ++i) 
	{
		RVO::Vector2 newP = sim->getAgentPosition(i) + sim->getAgentVelocity(i)*5.0f;
		points.push_back(newP);
	}
	std::vector<RVO::Segment> segments;
	segments.push_back(RVO::Segment(280,-200,280,200));
	segments.push_back(RVO::Segment(-280, -200, -280, 200));
	segments.push_back(RVO::Segment(-280, 200, 280, 200));
	segments.push_back(RVO::Segment(-280, -200, 280, -200));
	
	construct_voronoi(points.begin(), points.end(),segments.begin(), segments.end(), &vd);

	
			//visulize the voronoi diagram density
			//HRESULT hr = S_OK;
			//ID2D1GeometrySink *pSink = NULL;
			//if (SUCCEEDED(hr))
			//{
			//	hr = app.m_pDirect2dFactory->CreatePathGeometry(&app.m_pPathGeometry);

			//	if (SUCCEEDED(hr))
			//	{
			//		// Write to the path geometry using the geometry sink.
			//		hr = app.m_pPathGeometry->Open(&pSink);

			//		if (SUCCEEDED(hr))
			//		{
			//			if (edge->is_primary())
			//			{
			//				pSink->BeginFigure(
			//					D2D1::Point2F(edge->vertex1()->x(), edge->vertex1()->y()),
			//					D2D1_FIGURE_BEGIN_FILLED
			//					);
			//			}
			//		}
			//	}
			//}
			

			/*for (auto p: poly.outer())
			{
				pSink->AddLine(D2D1::Point2F(p.x(), p.y()));
			}
			pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
			hr = pSink->Close();
			SafeRelease(&pSink);*/


			

			
			/*auto c = voronoiDensity[index]>= 1 ? 1 : voronoiDensity[index];
			app.m_pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF::ColorF(c, 1.0 - c, 0, 0.3),
				&voronoiBrush
				);*/
			//app.m_pCompatibleRenderTarget->FillGeometry(app.m_pPathGeometry, voronoiBrush);

			
			/*fileOut.open("d://d2.txt", std::ofstream::app);
			fileOut << voronoiDensity[index] << std::endl;
			fileOut.close();*/
	

	
	
	
	//draw segment
	app.m_pCompatibleRenderTarget->DrawLine(D2D1::Point2F(280, -200),
		D2D1::Point2F(320, 240),
		app.m_pCornflowerBlueBrush,
		0.5f);
	app.m_pCompatibleRenderTarget->DrawLine(D2D1::Point2F(-280, -200),
		D2D1::Point2F(-280, 200),
		app.m_pCornflowerBlueBrush,
		0.5f);
	app.m_pCompatibleRenderTarget->DrawLine(D2D1::Point2F(-280, 200),
		D2D1::Point2F(280, 200),
		app.m_pCornflowerBlueBrush,
		0.5f);
	app.m_pCompatibleRenderTarget->DrawLine(D2D1::Point2F(-280, -200),
		D2D1::Point2F(280, -200),
		app.m_pCornflowerBlueBrush,
		0.5f);
	//draw voronoi diagram
	for (const_edge_iterator it = vd.edges().begin();
	it != vd.edges().end(); ++it) {
		if (!it->is_primary()) {
			continue;
		}
		if (it->vertex0() == NULL || it->vertex1() == NULL)
		{
			continue;
		}
		float x0 = it->vertex0()->x();
		float y0 = it->vertex0()->y();
		float x1 = it->vertex1()->x();
		float y1 = it->vertex1()->y();
		//point_type vertex0(it->vertex0()->x(), it->vertex0()->y());
		//point_type vertex1(it->vertex1()->x(), it->vertex1()->y());
		if(x0 != NULL && y0 != NULL && x1 != NULL && y1 != NULL)
		app.m_pCompatibleRenderTarget->DrawLine(D2D1::Point2F(x0, y0),
		D2D1::Point2F(x1, y1),
		app.m_pCornflowerBlueBrush,
		0.5f);
	}

	setPreferredVelocities(sim);
	seeForwardVoronoi(vd);
	
		
	D2D1_POINT_2U pt{ 0, 0 };
	D2D1_RECT_U rct{ 0, 0, wndWidth, wndHeight };
	app.m_pBitmap->CopyFromRenderTarget(&pt, app.m_pCompatibleRenderTarget, &rct);
	app.m_pCompatibleRenderTarget->EndDraw();
	app.m_pRenderTarget->BeginDraw();
	app.m_pRenderTarget->DrawBitmap(app.m_pBitmap);
	app.m_pRenderTarget->EndDraw();
	
	simClock.GetRenderElaspedTime();
	
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
						simClock.RecordUpdateCounter();
						simClock.CalcUpdateFPS();
						//setPreferredVelocities(sim);
						//dmap.seeForward();
						
						sim->doStep(); 
						//dmap.CalcDensity();
						//dmap.VelFromDensity(std::bind(&DensityMap::modelFunc1, &dmap, _1));
						simClock.GetUpdateElaspedTime();
						updateVisualization(sim);
						//
					}
					
				}
				
			}
		
		CoUninitialize();
	}
	//fileOut.open("d://circle2000d4.txt", std::ofstream::app);
	//fileOut << simClock.TotalRealTime() << std::endl;
	//fileOut.close();
	delete sim;
	return 0;
}
