#include "guis/modelGUIs/pointGUI.hpp"

#include "models/point.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <string>

PointGUI::PointGUI(Point& point) :
	ModelGUI{point},
	m_point{point}
{ }

void PointGUI::update()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";

	glm::vec3 pos = m_point.getPos();
	ImGui::InputFloat(("x" + suffix()).c_str(), &pos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("y" + suffix()).c_str(), &pos.y, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("z" + suffix()).c_str(), &pos.z, stepPrecision, stepPrecision,
		format.c_str());
	m_point.setPos(pos);
}
