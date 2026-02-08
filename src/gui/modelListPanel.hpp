#pragma once

#include "gui/guiMode.hpp"
#include "models/modelType.hpp"
#include "scene.hpp"

class ModelListPanel
{
public:
	ModelListPanel(Scene& scene);
	void update(GUIMode mode);

private:
	Scene& m_scene;
	ModelType m_modelType = ModelType::all;

	void updateModelType();
	void updateModelList(GUIMode mode);
};
