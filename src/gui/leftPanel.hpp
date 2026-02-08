#pragma once

#include "gui/addBezierSurfacePanel.hpp"
#include "gui/addGregorySurfacePanel.hpp"
#include "gui/addIntersectionPanel.hpp"
#include "gui/convertIntersectionToInterpolatingCurvePanel.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

class LeftPanel
{
	enum class Mode
	{
		none,
		c0BezierSurface,
		c2BezierSurface,
		gregorySurface,
		intersection,
		convert
	};

public:
	static constexpr int width = 272;

	LeftPanel(Scene& scene, const glm::ivec2& viewportSize);
	void update();

private:
	Scene& m_scene;
	const glm::ivec2& m_viewportSize;

	Mode m_mode = Mode::none;
	AddBezierSurfacePanel m_addC0BezierSurfacePanel;
	AddBezierSurfacePanel m_addC2BezierSurfacePanel;
	AddGregorySurfacePanel m_addGregorySurfacePanel;
	AddIntersectionPanel m_addIntersectionPanel;
	ConvertIntersectionToInterpolatingCurvePanel m_convertIntersectionToInterpolatingCurvePanel;

	void updateCamera();
	void updateAnaglyph();
	void updateCursor();
	void updateSelectedModelsCenter();
	void updateButtons();
	void resetPanels();

	void separator();
};
