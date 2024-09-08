#include "guis/modelGUIs/gregorySurfaceGUI.hpp"

#include "models/gregorySurface.hpp"

#include <imgui/imgui.h>

GregorySurfaceGUI::GregorySurfaceGUI(GregorySurface& surface) :
	ModelGUI{surface},
	m_surface{surface}
{ }

void GregorySurfaceGUI::update()
{
	bool renderVectors = m_surface.getRenderVectors();
	bool prevRenderVectors = renderVectors;
	ImGui::Checkbox(("render vectors" + suffix()).c_str(), &renderVectors);
	if (renderVectors != prevRenderVectors)
	{
		m_surface.setRenderVectors(renderVectors);
	}

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
