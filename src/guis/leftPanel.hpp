#pragma once

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
	
	std::string m_cameraType{};

	void updateCamera();
	void updateCursor();
	void updateSelectedModelsCenter();
	void updateButtons();
	void updateModelList(GUIMode mode);

	void separator();
};
