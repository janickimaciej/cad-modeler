#include "guis/valueWindows/renamingWindow.hpp"

#include <imgui/imgui.h>

RenamingWindow::RenamingWindow(Scene& scene, const glm::ivec2& viewportSize) :
	ValueWindow{viewportSize},
	m_scene{scene}
{ }

void RenamingWindow::startRenaming()
{
	std::string name = m_scene.getUniqueSelectedModelName();
	std::copy(name.begin(), name.end(), m_name.begin());
	m_name[name.size()] = '\0';
}

void RenamingWindow::apply()
{
	m_scene.setUniqueSelectedModelName({m_name.data()});
}

std::string RenamingWindow::name()
{
	return "renaming";
}

int RenamingWindow::width()
{
	return 250;
}

void RenamingWindow::input()
{
	ImGui::InputText("##renaming", m_name.data(), m_name.size());
}
