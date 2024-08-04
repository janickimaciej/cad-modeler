#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierCurveC0;

class BezierCurveC0GUI : public ModelGUI
{
public:
	BezierCurveC0GUI(BezierCurveC0& curve);
	virtual void update() override;

private:
	BezierCurveC0& m_curve;
};
