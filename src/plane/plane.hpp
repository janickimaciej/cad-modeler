#pragma once

#include "cameras/cameraType.hpp"
#include "quad.hpp"

class Plane
{
public:
	Plane(float scale);
	void render(CameraType cameraType) const;

private:
	float m_scale{};
	Quad m_quad{};

	void updateShaders(CameraType cameraType) const;
};
