#include "guis/centerPointGUI.hpp"

#include "centerPoint.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <string>

CenterPointGUI::CenterPointGUI(CenterPoint& centerPoint) :
	m_centerPoint{centerPoint}
{ }

void CenterPointGUI::update()
{
	static const std::string suffix = "##centerPoint";

	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";

	glm::vec3 pos = m_centerPoint.getPos();
	glm::vec3 prevPos = pos;
	ImGui::InputFloat(("x" + suffix).c_str(), &pos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("y" + suffix).c_str(), &pos.y, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("z" + suffix).c_str(), &pos.z, stepPrecision, stepPrecision,
		format.c_str());
	if (pos != prevPos)
	{
		m_centerPoint.setPos(pos);
	}
}
