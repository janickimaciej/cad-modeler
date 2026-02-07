#pragma once

#include "guis/guiMode.hpp"
#include "guis/modelListPanel.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

class RightPanel
{
public:
	static constexpr int width = 350;

	RightPanel(Scene& scene, const glm::ivec2& viewportSize);
	void update(GUIMode mode);

private:
	const glm::ivec2& m_viewportSize;

	ModelListPanel m_modelListPanel;
};
