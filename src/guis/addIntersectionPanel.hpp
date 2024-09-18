#pragma once

#include "intersectable.hpp"
#include "scene.hpp"

#include <array>
#include <functional>

class AddIntersectionPanel
{
	using Callback = std::function<void()>;

public:
	AddIntersectionPanel(Scene& scene, const Callback& callback);
	void reset();
	void update();

private:
	Scene& m_scene;
	Callback m_callback{};

	std::array<const Intersectable*, 2> m_surfaces{};
	int m_surfaceCount = 0;
	bool m_useCursor = false;
};
