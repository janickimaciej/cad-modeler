#include "guis/modelGUIs/bezierSurfaceGUI.hpp"

#include "models/bezierSurfaces/bezierSurface.hpp"

#include <imgui/imgui.h>

BezierSurfaceGUI::BezierSurfaceGUI(BezierSurface& surface) :
	ModelGUI{surface},
	m_surface{surface},
	m_intersectableGUI{surface}
{ }

void BezierSurfaceGUI::update()
{
	updateRenderGrid();
	updateLineCount();
	m_intersectableGUI.update();
}

void BezierSurfaceGUI::updateRenderGrid()
{
	bool renderGrid = m_surface.getRenderGrid();
	bool prevRenderGrid = renderGrid;
	ImGui::Checkbox(("render grid" + suffix()).c_str(), &renderGrid);
	if (renderGrid != prevRenderGrid)
	{
		m_surface.setRenderGrid(renderGrid);
	}
}

void BezierSurfaceGUI::updateLineCount()
{
	static constexpr int stepPrecision = 1;

	int lineCount = m_surface.getLineCount();
	int prevLineCount = lineCount;
	ImGui::InputInt(("line count" + suffix()).c_str(), &lineCount, stepPrecision, stepPrecision);
	lineCount = std::max(lineCount, 2);
	lineCount = std::min(lineCount, 64);
	if (lineCount != prevLineCount)
	{
		m_surface.setLineCount(lineCount);
	}
}
