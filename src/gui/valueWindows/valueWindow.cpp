#include "gui/valueWindows/valueWindow.hpp"

#include "gui/leftPanel.hpp"
#include "gui/rightPanel.hpp"

#include <imgui/imgui.h>

ValueWindow::ValueWindow(const glm::ivec2& viewportSize) :
	m_viewportSize{viewportSize}
{ }

void ValueWindow::update()
{
	static constexpr int height = 35;
	int windowWidth = LeftPanel::width + m_viewportSize.x + RightPanel::width;
	ImGui::SetNextWindowPos({(windowWidth - width()) / 2.0f, (m_viewportSize.y - height) / 2.0f},
		ImGuiCond_Always);
	ImGui::SetNextWindowSize({static_cast<float>(width()), static_cast<float>(height)},
		ImGuiCond_Always);
	ImGui::Begin(name().c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

	ImGui::PushItemWidth(static_cast<float>(width() - 17));
	ImGui::SetKeyboardFocusHere();
	input();
	ImGui::PopItemWidth();

	ImGui::End();
}
