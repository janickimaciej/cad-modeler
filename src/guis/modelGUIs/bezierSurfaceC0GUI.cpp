#include "guis/modelGUIs/bezierSurfaceC0GUI.hpp"

#include "models/bezierSurfaces/bezierSurfaceC0.hpp"

#include <imgui/imgui.h>

BezierSurfaceC0GUI::BezierSurfaceC0GUI(BezierSurfaceC0& surface) :
	ModelGUI{surface},
	m_surface{surface}
{ }

void BezierSurfaceC0GUI::update()
{
	bool renderGrid = m_surface.getRenderGrid();
	bool prevRenderGrid = renderGrid;
	ImGui::Checkbox(("render grid" + suffix()).c_str(), &renderGrid);
	if (renderGrid != prevRenderGrid)
	{
		m_surface.setRenderGrid(renderGrid);
	}
}
