#pragma once

#include "guis/addBezierSurfacePanel.hpp"
#include "guis/addGregorySurfacePanel.hpp"
#include "guis/addIntersectionPanel.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

class LeftPanel
{
	enum class Mode
	{
		none,
		bezierSurfaceC0,
		bezierSurfaceC2,
		gregorySurface,
		intersection
	};

public:
	static constexpr int width = 250;

	LeftPanel(Scene& scene, const glm::ivec2& windowSize);
	void update();

private:
	Scene& m_scene;
	const glm::ivec2& m_windowSize;

	Mode m_mode = Mode::none;
	AddBezierSurfacePanel m_addBezierSurfaceC0Panel;
	AddBezierSurfacePanel m_addBezierSurfaceC2Panel;
	AddGregorySurfacePanel m_addGregorySurfacePanel;
	AddIntersectionPanel m_addIntersectionPanel;

	void updateCamera();
	void updateAnaglyph();
	void updateCursor();
	void updateSelectedModelsCenter();
	void updateButtons();
	void resetPanels();

	void separator();
};
