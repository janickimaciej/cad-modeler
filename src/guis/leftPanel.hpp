#pragma once

#include "cameras/cameraType.hpp"
#include "guis/addBezierSurfacePanel.hpp"
#include "guis/guiMode.hpp"
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
	
	CameraType m_cameraType{};

	bool m_addingBezierSurfaceC0 = false;
	bool m_addingBezierSurfaceC2 = false;
	AddBezierSurfacePanel m_addBezierSurfaceC0Panel;
	AddBezierSurfacePanel m_addBezierSurfaceC2Panel;

	void updateCamera();
	void updateCursor();
	void updateSelectedModelsCenter();
	void updateButtons();
	void updateModelList(GUIMode mode);

	void separator();
};
