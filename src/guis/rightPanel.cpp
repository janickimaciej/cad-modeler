#include "guis/rightPanel.hpp"

#include <imgui/imgui.h>

RightPanel::RightPanel(Scene& scene, const glm::ivec2& windowSize) :
	m_windowSize{windowSize},
	m_modelListPanel{scene}
{ }

void RightPanel::update(GUIMode mode)
{
	ImGui::SetNextWindowPos({static_cast<float>(m_windowSize.x - width), 0}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({width, static_cast<float>(m_windowSize.y)},
		ImGuiCond_Always);
	ImGui::Begin("rightPanel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground);
	ImGui::PushItemWidth(100);

	m_modelListPanel.update(mode);

	ImGui::PopItemWidth();
	ImGui::End();
}
