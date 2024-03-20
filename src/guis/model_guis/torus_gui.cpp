#include "guis/model_guis/torus_gui.hpp"

#include "models/torus.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>
#include <string>

TorusGUI::TorusGUI(Torus& torus) :
	m_torus{torus}
{ }

void TorusGUI::update()
{
	const std::string suffix = "##" + m_torus.getOriginalName();

	getValues();

	ImGui::InputFloat(("x" + suffix).c_str(), &m_x, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("y" + suffix).c_str(), &m_y, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("z" + suffix).c_str(), &m_z, 0.1f, 0.1f, "%.2f");

	ImGui::InputFloat(("scale x" + suffix).c_str(), &m_scaleX, 0.1f, 0.1f, "%.2f");
	m_scaleX = std::max(m_scaleX, 0.1f);

	ImGui::InputFloat(("scale y" + suffix).c_str(), &m_scaleY, 0.1f, 0.1f, "%.2f");
	m_scaleY = std::max(m_scaleY, 0.1f);

	ImGui::InputFloat(("scale z" + suffix).c_str(), &m_scaleZ, 0.1f, 0.1f, "%.2f");
	m_scaleZ = std::max(m_scaleZ, 0.1f);

	ImGui::InputFloat(("major radius" + suffix).c_str(), &m_majorRadius, 0.1f, 0.1f, "%.2f");
	m_majorRadius = std::max(m_majorRadius, 0.1f);

	ImGui::InputFloat(("minor radius" + suffix).c_str(), &m_minorRadius, 0.1f, 0.1f, "%.2f");
	m_minorRadius = std::max(m_minorRadius, 0.1f);

	ImGui::InputInt(("major" + suffix).c_str(), &m_major, 1, 1);
	m_major = std::max(m_major, 3);

	ImGui::InputInt(("minor" + suffix).c_str(), &m_minor, 1, 1);
	m_minor = std::max(m_minor, 3);

	setValues();
}

void TorusGUI::getValues()
{
	glm::vec3 position = m_torus.getPosition();
	m_x = position.x;
	m_y = position.y;
	m_z = position.z;
	glm::vec3 scale = m_torus.getScale();
	m_scaleX = scale.x;
	m_scaleY = scale.y;
	m_scaleZ = scale.z;
	m_majorRadius = m_torus.getMajorRadius();
	m_minorRadius = m_torus.getMinorRadius();
	m_major = m_torus.getMajor();
	m_minor = m_torus.getMinor();
}

void TorusGUI::setValues()
{
	m_torus.setPosition(glm::vec3{m_x, m_y, m_z});
	m_torus.setScale(glm::vec3{m_scaleX, m_scaleY, m_scaleZ});
	m_torus.setMajorRadius(m_majorRadius);
	m_torus.setMinorRadius(m_minorRadius);
	m_torus.setMajor(m_major);
	m_torus.setMinor(m_minor);
}
