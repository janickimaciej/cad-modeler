#include "guis/addBezierSurfacePanel.hpp"

#include <imgui/imgui.h>

#include <algorithm>

AddBezierSurfacePanel::AddBezierSurfacePanel(const Callback& callback) :
	m_suffix{"##AddBezierSurfacePanel " + std::to_string(m_count++)},
	m_callback{callback}
{
	reset();
}

void AddBezierSurfacePanel::reset()
{
	m_patchesU = 2;
	m_patchesV = 2;
	m_sizeU = 6;
	m_sizeV = 6;
	m_wrapping = BezierSurfaceWrapping::none;
}

void AddBezierSurfacePanel::update()
{
	updatePatches();
	updateSize();
	updateWrapping();

	if (ImGui::Button(("Apply" + m_suffix).c_str()))
	{
		m_callback(m_patchesU, m_patchesV, m_sizeU, m_sizeV, m_wrapping);
	}
}

int AddBezierSurfacePanel::m_count = 0;

void AddBezierSurfacePanel::updatePatches()
{
	static constexpr int stepPrecision = 1;

	int minPatchesU{};
	int minPatchesV{};
	switch (m_wrapping)
	{
		case BezierSurfaceWrapping::none:
			minPatchesU = 1;
			minPatchesV = 1;
			break;
		case BezierSurfaceWrapping::u:
			minPatchesU = 3;
			minPatchesV = 1;
			break;
		case BezierSurfaceWrapping::v:
			minPatchesU = 1;
			minPatchesV = 3;
			break;
	}

	ImGui::InputInt(("patches U" + m_suffix).c_str(), &m_patchesU, stepPrecision,
		stepPrecision);
	m_patchesU = std::max(m_patchesU, minPatchesU);

	ImGui::InputInt(("patches V" + m_suffix).c_str(), &m_patchesV, stepPrecision,
		stepPrecision);
	m_patchesV = std::max(m_patchesV, minPatchesV);
}

void AddBezierSurfacePanel::updateSize()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";
	
	float* firstVariable = &m_sizeU;
	float* secondVariable = &m_sizeV;
	std::string firstLabel = "size U";
	std::string secondLabel = "size V";
	{
		switch (m_wrapping)
		{
			case BezierSurfaceWrapping::u:
				firstLabel = "radius";
				secondLabel = "height";
				break;

			case BezierSurfaceWrapping::v:
				firstVariable = &m_sizeV;
				secondVariable = &m_sizeU;
				firstLabel = "radius";
				secondLabel = "height";
				break;
		}
	}
	ImGui::InputFloat((firstLabel + m_suffix).c_str(), firstVariable, stepPrecision, stepPrecision,
		format.c_str());
	*firstVariable = std::max(*firstVariable, 0.1f);
	ImGui::InputFloat((secondLabel + m_suffix).c_str(), secondVariable, stepPrecision,
		stepPrecision, format.c_str());
	*secondVariable = std::max(*secondVariable, 0.1f);
}

void AddBezierSurfacePanel::updateWrapping()
{
	if (ImGui::BeginCombo((m_suffix + " wrapping").c_str(),
		bezierSurfaceWrappingLabels[static_cast<int>(m_wrapping)].c_str()))
	{
		for (int wrapping = 0; wrapping < bezierSurfaceWrappingCount; ++wrapping)
		{
			bool isSelected = wrapping == static_cast<int>(m_wrapping);
			if (ImGui::Selectable(bezierSurfaceWrappingLabels[wrapping].c_str(), isSelected))
			{
				m_wrapping = static_cast<BezierSurfaceWrapping>(wrapping);
			}
		}
		ImGui::EndCombo();
	}
	ImGui::SameLine();
	ImGui::Text("wrapping");
}
