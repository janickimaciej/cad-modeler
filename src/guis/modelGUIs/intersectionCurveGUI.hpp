#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class IntersectionCurve;

class IntersectionCurveGUI : public ModelGUI
{
public:
	IntersectionCurveGUI(IntersectionCurve& curve);
	void update();

private:
	IntersectionCurve& m_curve;
};
