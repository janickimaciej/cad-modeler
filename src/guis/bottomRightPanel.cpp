#include "guis/bottomRightPanel.hpp"

#include <imgui/imgui.h>

BottomRightPanel::BottomRightPanel(const glm::ivec2& windowSize) :
	m_windowSize{windowSize}
{ }

void BottomRightPanel::update()
{
	ImGui::SetNextWindowPos({static_cast<float>(m_windowSize.x - width),
		static_cast<float>(m_windowSize.y - width)}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({width, width},
		ImGuiCond_Always);
	ImGui::Begin("bottomRightPanel", nullptr,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushItemWidth(100);

	float imageSize = width - 16;
	ImGui::Image(nullptr, {imageSize, imageSize});

	ImGui::PopItemWidth();
	ImGui::End();
}
