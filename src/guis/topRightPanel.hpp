#pragma once

#include "guis/guiMode.hpp"
#include "guis/modelListPanel.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

class TopRightPanel
{
public:
	static constexpr int width = 272;

	TopRightPanel(Scene& scene, const glm::ivec2& windowSize);
	void update(GUIMode mode);

private:
	const glm::ivec2& m_windowSize;

	ModelListPanel m_modelListPanel;
};
