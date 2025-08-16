#pragma once

#include "models/intersectable.hpp"

class IntersectableGUI
{
public:
	IntersectableGUI(Intersectable& intersectable);
	void update();

private:
	Intersectable& m_intersectable;
};
