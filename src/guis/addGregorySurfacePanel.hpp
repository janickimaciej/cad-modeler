#pragma once

#include "scene.hpp"

#include <array>
#include <optional>
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

	std::array<int, 3> m_patches{};
	int m_chosenPatches = 0;

	void updateList(std::optional<int>& clickedPatch);
	void updateButton(std::optional<int> clickedPatch);
};
