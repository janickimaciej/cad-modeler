#pragma once

#include "models/modelType.hpp"
#include "scene.hpp"

#include <array>
#include <optional>
#include <functional>

class AddIntersectionPanel
{
	using Callback = std::function<void()>;

public:
	AddIntersectionPanel(Scene& scene, const Callback& callback);
	void start();
	void reset();
	void update();

private:
	Scene& m_scene;
	Callback m_callback{};

	std::array<ModelType, 2> m_types{};
	std::array<int, 2> m_surfaces{};
	int m_chosenSurfaces = 0;

	void updateList(std::optional<ModelType>& clickedType, std::optional<int>& clickedSurface);
	void updateList(std::optional<ModelType>& clickedType, std::optional<int>& clickedSurface,
		ModelType type);
	void updateButton(std::optional<ModelType> clickedType, std::optional<int> clickedSurface);
};
