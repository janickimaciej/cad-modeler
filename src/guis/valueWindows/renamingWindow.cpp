#include "guis/valueWindows/renamingWindow.hpp"

#include <imgui/imgui.h>

RenamingWindow::RenamingWindow(Scene& scene, const glm::ivec2& windowSize) :
	ValueWindow{windowSize},
	m_scene{scene}
{ }

bool RenamingWindow::startRenaming()
{
	m_uniqueActiveModel = m_scene.getUniqueActiveModel();
	if (m_uniqueActiveModel != nullptr)
	{
		std::string name = m_uniqueActiveModel->getName();
		std::copy(name.begin(), name.end(), m_name.begin());
		m_name[name.size()] = '\0';

		return true;
	}
	return false;
}

void RenamingWindow::cancel()
{
	m_uniqueActiveModel = nullptr;
}

void RenamingWindow::apply()
{
	m_uniqueActiveModel->setName(std::string{m_name.data()});
	m_uniqueActiveModel = nullptr;
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
