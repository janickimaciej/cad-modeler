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
	glm::vec3 prevPos = pos;
	ImGui::InputFloat(("x" + suffix()).c_str(), &pos.x, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("y" + suffix()).c_str(), &pos.y, stepPrecision, stepPrecision,
		format.c_str());
	ImGui::InputFloat(("z" + suffix()).c_str(), &pos.z, stepPrecision, stepPrecision,
		format.c_str());
	if (pos != prevPos)
	{
		m_torus.setPos(pos);
	}
}

void TorusGUI::updateOrientation()
{
	static constexpr float stepPrecision = 1;
	static const std::string format = "%.1f";

	float pitchDeg = glm::degrees(m_torus.getPitchRad());
	float prevPitchDeg = pitchDeg;
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
	if (pitchDeg != prevPitchDeg)
	{
		m_torus.setPitchRad(glm::radians(pitchDeg));
	}

	float yawDeg = glm::degrees(m_torus.getYawRad());
	float prevYawDeg = yawDeg;
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
	if (yawDeg != prevYawDeg)
	{
		m_torus.setYawRad(glm::radians(yawDeg));
	}
	
	float rollDeg = glm::degrees(m_torus.getRollRad());
	float prevRollDeg = rollDeg;
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
	if (rollDeg != prevRollDeg)
	{
		m_torus.setRollRad(glm::radians(rollDeg));
	}
}

void TorusGUI::updateScale()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";

	glm::vec3 scale = m_torus.getScale();
	glm::vec3 prevScale = scale;

	ImGui::InputFloat(("scale x" + suffix()).c_str(), &scale.x, stepPrecision, stepPrecision,
		format.c_str());
	scale.x = std::max(scale.x, 0.1f);

	ImGui::InputFloat(("scale y" + suffix()).c_str(), &scale.y, stepPrecision, stepPrecision,
		format.c_str());
	scale.y = std::max(scale.y, 0.1f);

	ImGui::InputFloat(("scale z" + suffix()).c_str(), &scale.z, stepPrecision, stepPrecision,
		format.c_str());
	scale.z = std::max(scale.z, 0.1f);

	if (scale != prevScale)
	{
		m_torus.setScale(scale);
	}
}

void TorusGUI::updateRadii()
{
	static constexpr float stepPrecision = 0.1f;
	static const std::string format = "%.2f";
	
	float majorRadius = m_torus.getMajorRadius();
	float prevMajorRadius = majorRadius;
	ImGui::InputFloat(("major radius" + suffix()).c_str(), &majorRadius, stepPrecision,
		stepPrecision, format.c_str());
	majorRadius = std::max(majorRadius, 0.1f);
	if (majorRadius != prevMajorRadius)
	{
		m_torus.setMajorRadius(majorRadius);
	}
	
	float minorRadius = m_torus.getMinorRadius();
	float prevMinorRadius = minorRadius;
	ImGui::InputFloat(("minor radius" + suffix()).c_str(), &minorRadius, stepPrecision,
		stepPrecision, format.c_str());
	minorRadius = std::max(minorRadius, 0.1f);
	if (minorRadius != prevMinorRadius)
	{
		m_torus.setMinorRadius(minorRadius);
	}
}

void TorusGUI::updateGrid()
{
	static constexpr int stepPrecision = 1;

	int majorGrid = m_torus.getMajorGrid();
	int prevMajorGrid = majorGrid;
	ImGui::InputInt(("major grid" + suffix()).c_str(), &majorGrid, stepPrecision, stepPrecision);
	majorGrid = std::max(majorGrid, 3);
	if (majorGrid != prevMajorGrid)
	{
		m_torus.setMajorGrid(majorGrid);
	}

	int minorGrid = m_torus.getMinorGrid();
	int prevMinorGrid = minorGrid;
	ImGui::InputInt(("minor grid" + suffix()).c_str(), &minorGrid, stepPrecision, stepPrecision);
	minorGrid = std::max(minorGrid, 3);
	if (minorGrid != prevMinorGrid)
	{
		m_torus.setMinorGrid(minorGrid);
	}
}
