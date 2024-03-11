#include "model_guis/torus_gui.hpp"

#include "models/torus.hpp"

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

#include <algorithm>

TorusGUI::TorusGUI(Torus& torus) :
	m_torus{torus}
{ }

void TorusGUI::update()
{
	getValues();

	ImGui::InputFloat("major radius", &m_majorRadius, 0.1f, 0.1f, "%.2f");
	m_majorRadius = std::max(m_majorRadius, 0.1f);

	ImGui::InputFloat("minor radius", &m_minorRadius, 0.1f, 0.1f, "%.2f");
	m_minorRadius = std::max(m_minorRadius, 0.1f);

	ImGui::InputInt("major", &m_major, 1, 1);
	m_major = std::max(m_major, 3);

	ImGui::InputInt("minor", &m_minor, 1, 1);
	m_minor = std::max(m_minor, 3);

	setValues();
}

void TorusGUI::getValues()
{
	m_majorRadius = m_torus.getMajorRadius();
	m_minorRadius = m_torus.getMinorRadius();
	m_major = m_torus.getMajor();
	m_minor = m_torus.getMinor();
}

void TorusGUI::setValues()
{
	m_torus.setMajorRadius(m_majorRadius);
	m_torus.setMinorRadius(m_minorRadius);
	m_torus.setMajor(m_major);
	m_torus.setMinor(m_minor);
}
