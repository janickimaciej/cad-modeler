#pragma once

#include "models/modelType.hpp"
#include "scene.hpp"

#include <array>
#include <functional>

class AddGregorySurfacePanel
{
	using Callback = std::function<void()>;

public:
	AddGregorySurfacePanel(Scene& scene, const Callback& callback);
	void start();
	void reset();
	void update();

private:
	Scene& m_scene;
	Callback m_callback{};

	std::array<ModelType, 3> m_types{};
	std::array<int, 3> m_surfaces{};
	std::array<int, 3> m_patches{};
	int m_chosenPatches = 0;

	void update(ModelType type, std::optional<ModelType>& clickedType,
		std::optional<int>& clickedSurface, std::optional<int>& clickedPatch);
};
