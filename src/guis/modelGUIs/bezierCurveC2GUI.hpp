#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierCurveC2;

class BezierCurveC2GUI : public ModelGUI
{
public:
	BezierCurveC2GUI(BezierCurveC2& curve);
	virtual void update() override;

private:
	BezierCurveC2& m_curve;
};
