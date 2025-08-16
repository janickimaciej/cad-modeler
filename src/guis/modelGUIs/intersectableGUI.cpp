#include "guis/modelGUIs/intersectableGUI.hpp"

#include <imgui/imgui.h>

#include <optional>

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

		std::optional<int> clickedCurve = std::nullopt;
		std::optional<int> selectedCurve = m_intersectable.selectedIntersectionCurveIndex();
		for (int i = 0; i < m_intersectable.intersectionCurveCount(); ++i)
		{
			ImGuiTreeNodeFlags flags = globalFlags;
			if (selectedCurve.has_value() && i == *selectedCurve)
			{
				flags |= ImGuiTreeNodeFlags_Selected;
			}

			bool isOpen = ImGui::TreeNodeEx((m_intersectable.intersectionCurveName(i) + "##" +
				m_intersectable.getOriginalName() + std::to_string(i)).c_str(), flags);
			if (ImGui::IsItemClicked())
			{
				clickedCurve = i;
			}
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
				ImGui::TreePop();
			}
		}

		if (clickedCurve.has_value())
		{
			m_intersectable.selectIntersectionCurve(*clickedCurve);
		}
	}
}
