#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierCurve;

class BezierCurveGUI : public ModelGUI
{
public:
	BezierCurveGUI(BezierCurve& curve);
	virtual void update() override;

private:
	BezierCurve& m_curve;
};
