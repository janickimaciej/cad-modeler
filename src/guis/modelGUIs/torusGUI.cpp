#include "guis/modelGUIs/torusGUI.hpp"

#include "models/torus.hpp"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

#include <algorithm>
#include <string>

TorusGUI::TorusGUI(Torus& torus) :
	ModelGUI{torus},
	m_torus{torus}
{ }

void TorusGUI::update()
{
	updatePos();
	updateOrientation();
	updateScale();
	updateRadii();
	updateGrid();
}

void TorusGUI::updatePos()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";

	glm::vec3 pos = m_torus.getPos();
	ImGui::InputFloat(("x" + suffix()).c_str(), &pos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("y" + suffix()).c_str(), &pos.y, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("z" + suffix()).c_str(), &pos.z, stepPrecision, stepPrecision,
		format.c_str());
	m_torus.setPos(pos);
}

void TorusGUI::updateOrientation()
{
	static constexpr float stepPrecision = 1;
	static const std::string format = "%.1f";

	float pitchDeg = glm::degrees(m_torus.getPitchRad());
	ImGui::InputFloat(("pitch" + suffix()).c_str(), &pitchDeg, stepPrecision, stepPrecision,
		format.c_str());
	if (pitchDeg < -90)
	{
		pitchDeg = -90;
	}
	if (pitchDeg > 90)
	{
		pitchDeg = 90;
	}
	m_torus.setPitchRad(glm::radians(pitchDeg));

	float yawDeg = glm::degrees(m_torus.getYawRad());
	ImGui::InputFloat(("yaw" + suffix()).c_str(), &yawDeg, stepPrecision, stepPrecision,
		format.c_str());
	while (yawDeg < -180)
	{
		yawDeg += 360;
	}
	while (yawDeg > 180)
	{
		yawDeg -= 360;
	}
	m_torus.setYawRad(glm::radians(yawDeg));
	
	float rollDeg = glm::degrees(m_torus.getRollRad());
	ImGui::InputFloat(("roll" + suffix()).c_str(), &rollDeg, stepPrecision, stepPrecision,
		format.c_str());
	while (rollDeg < -180)
	{
		rollDeg += 360;
	}
	while (rollDeg > 180)
	{
		rollDeg -= 360;
	}
	m_torus.setRollRad(glm::radians(rollDeg));
}

void TorusGUI::updateScale()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";

	glm::vec3 scale = m_torus.getScale();

	ImGui::InputFloat(("scale x" + suffix()).c_str(), &scale.x, stepPrecision, stepPrecision,
		format.c_str());
	scale.x = std::max(scale.x, 0.1f);

	ImGui::InputFloat(("scale y" + suffix()).c_str(), &scale.y, stepPrecision, stepPrecision,
		format.c_str());
	scale.y = std::max(scale.y, 0.1f);

	ImGui::InputFloat(("scale z" + suffix()).c_str(), &scale.z, stepPrecision, stepPrecision,
		format.c_str());
	scale.z = std::max(scale.z, 0.1f);

	m_torus.setScale(scale);
}

void TorusGUI::updateRadii()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";
	
	float majorRadius = m_torus.getMajorRadius();
	ImGui::InputFloat(("major radius" + suffix()).c_str(), &majorRadius, stepPrecision,
		stepPrecision, format.c_str());
	majorRadius = std::max(majorRadius, 0.1f);
	m_torus.setMajorRadius(majorRadius);
	
	float minorRadius = m_torus.getMinorRadius();
	ImGui::InputFloat(("minor radius" + suffix()).c_str(), &minorRadius, stepPrecision,
		stepPrecision, format.c_str());
	minorRadius = std::max(minorRadius, 0.1f);
	m_torus.setMinorRadius(minorRadius);
}

void TorusGUI::updateGrid()
{
	static constexpr int stepPrecision = 1;

	int majorGrid = m_torus.getMajorGrid();
	ImGui::InputInt(("major grid" + suffix()).c_str(), &majorGrid, stepPrecision, stepPrecision);
	majorGrid = std::max(majorGrid, 3);
	m_torus.setMajorGrid(majorGrid);

	int minorGrid = m_torus.getMinorGrid();
	ImGui::InputInt(("minor grid" + suffix()).c_str(), &minorGrid, stepPrecision, stepPrecision);
	minorGrid = std::max(minorGrid, 3);
	m_torus.setMinorGrid(minorGrid);
}
