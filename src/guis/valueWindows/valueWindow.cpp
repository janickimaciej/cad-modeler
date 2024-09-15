#include "guis/valueWindows/valueWindow.hpp"

#include <imgui/imgui.h>

ValueWindow::ValueWindow(const glm::ivec2& windowSize) :
	m_windowSize{windowSize}
{ }

void ValueWindow::update()
{
	static constexpr int height = 35;
	ImGui::SetNextWindowPos({(m_windowSize.x - width() + 50) / 2.0f,
		(m_windowSize.y - height) / 2.0f}, ImGuiCond_Always);
	ImGui::SetNextWindowSize({static_cast<float>(width()), static_cast<float>(height)},
		ImGuiCond_Always);
	ImGui::Begin(name().c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	ImGui::PushItemWidth(static_cast<float>(width() - 17));
	ImGui::SetKeyboardFocusHere();
	input();
	ImGui::PopItemWidth();

	ImGui::End();
}
