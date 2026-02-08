#pragma once

#include "models/bezierSurfaces/bezierSurfaceWrapping.hpp"

#include <functional>
#include <string>

class AddBezierSurfacePanel
{
	using Callback = std::function<void(int, int, float, float, BezierSurfaceWrapping)>;

public:
	AddBezierSurfacePanel(const Callback& callback);
	void reset();
	void update();

private:
	static int m_count;
	std::string m_suffix{};

	Callback m_callback{};

	int m_patchesU{};
	int m_patchesV{};
	float m_sizeU{};
	float m_sizeV{};
	BezierSurfaceWrapping m_wrapping{};

	void updatePatches();
	void updateSize();
	void updateWrapping();
};
