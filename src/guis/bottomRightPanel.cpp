#include "guis/bottomRightPanel.hpp"

#include <imgui/imgui.h>

BottomRightPanel::BottomRightPanel(const glm::ivec2& windowSize) :
	m_windowSize{windowSize}
{ }

void BottomRightPanel::update()
{
	ImGui::SetNextWindowPos({static_cast<float>(m_windowSize.x - width),
		static_cast<float>(m_windowSize.y - 2 * width + 10)}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({width, static_cast<float>(2 * width - 10)},
		ImGuiCond_Always);
	ImGui::Begin("bottomRightPanel", nullptr,
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	ImGui::PushItemWidth(100);

	float imageSize = width - 15;
	ImGui::Image(nullptr, {imageSize, imageSize});
	ImGui::Image(nullptr, {imageSize, imageSize});

	ImGui::PopItemWidth();
	ImGui::End();
}
