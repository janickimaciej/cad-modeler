#pragma once

#include "scene.hpp"

#include <functional>

class ConvertIntersectionToInterpolatingCurvePanel
{
	using Callback = std::function<void()>;

public:
	ConvertIntersectionToInterpolatingCurvePanel(Scene& scene, const Callback& callback);
	void reset();
	void update();

private:
	Scene& m_scene;
	Callback m_callback{};

	int m_numberOfPoints{};
};
