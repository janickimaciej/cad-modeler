#include "guis/modelGUIs/intersectableGUI.hpp"

#include <imgui/imgui.h>

#include <cstdint>

IntersectableGUI::IntersectableGUI(Intersectable& intersectable) :
	m_intersectable{intersectable}
{ }

void IntersectableGUI::update()
{
	if (m_intersectable.intersectionCurveCount() > 0)
	{
		ImGui::Text("Intersection curves:");

		static constexpr ImGuiTreeNodeFlags globalFlags =
			ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		for (int i = 0; i < m_intersectable.intersectionCurveCount(); ++i)
		{
			ImGuiTreeNodeFlags flags = globalFlags;

			bool isOpen = ImGui::TreeNodeEx((m_intersectable.intersectionCurveName(i) + "##" +
				m_intersectable.getOriginalName() + std::to_string(i)).c_str(), flags);
			if (isOpen)
			{
				int trim = static_cast<int>(m_intersectable.getIntersectionCurveTrim(i));
				int prevTrim = trim;
				ImGui::Text("Trim:");
				ImGui::RadioButton("none", &trim, static_cast<int>(Intersectable::Trim::none));
				ImGui::SameLine();
				ImGui::RadioButton("red", &trim, static_cast<int>(Intersectable::Trim::red));
				ImGui::SameLine();
				ImGui::RadioButton("blue", &trim, static_cast<int>(Intersectable::Trim::blue));
				if (trim != prevTrim)
				{
					m_intersectable.setIntersectionCurveTrim(i,
						static_cast<Intersectable::Trim>(trim));
				}

				static constexpr int imageSize = 256;
				ImGui::Image
				(
					reinterpret_cast<void*>(static_cast<std::intptr_t>(
						m_intersectable.getIntersectionCurveTextureId(i))),
					{imageSize, imageSize}
				);
				ImGui::TreePop();
			}
		}
	}
}
