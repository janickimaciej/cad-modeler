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

	std::array<BezierPatch*, 3> m_patches{};
	int m_patchCount = 0;
};
