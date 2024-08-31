#pragma once

#include "guis/addBezierSurfacePanel.hpp"
#include "guis/guiMode.hpp"
#include "guis/modelListPanel.hpp"
#include "scene.hpp"

#include <string>

#include <glm/glm.hpp>

class LeftPanel
{
public:
	LeftPanel(Scene& scene, const glm::ivec2& windowSize);
	void update(GUIMode mode);

private:
	Scene& m_scene;
	const glm::ivec2& m_windowSize;

	bool m_addingBezierSurfaceC0 = false;
	bool m_addingBezierSurfaceC2 = false;
	AddBezierSurfacePanel m_addBezierSurfaceC0Panel;
	AddBezierSurfacePanel m_addBezierSurfaceC2Panel;
	ModelListPanel m_modelListPanel;

	void updateCamera();
	void updateAnaglyph();
	void updateCursor();
	void updateSelectedModelsCenter();
	void updateButtons();

	void separator();
};
