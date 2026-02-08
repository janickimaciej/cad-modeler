#pragma once

#include "gui/modelGUIs/modelGUI.hpp"

class IntersectionCurve;

class IntersectionCurveGUI : public ModelGUI
{
public:
	IntersectionCurveGUI(IntersectionCurve& curve);
	virtual ~IntersectionCurveGUI() = default;

	void update();

private:
	IntersectionCurve& m_curve;
};
