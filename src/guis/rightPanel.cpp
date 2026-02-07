#include "guis/rightPanel.hpp"

#include <imgui/imgui.h>

RightPanel::RightPanel(Scene& scene, const glm::ivec2& viewportSize) :
	m_viewportSize{viewportSize},
	m_modelListPanel{scene}
{ }

void RightPanel::update(GUIMode mode)
{
	ImGui::SetNextWindowPos({static_cast<float>(m_viewportSize.x - width), 0}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({width, static_cast<float>(m_viewportSize.y)},
		ImGuiCond_Always);
	ImGui::Begin("rightPanel", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushItemWidth(100);

	m_modelListPanel.update(mode);

	ImGui::PopItemWidth();
	ImGui::End();
}
