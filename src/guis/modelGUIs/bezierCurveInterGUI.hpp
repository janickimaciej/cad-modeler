#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierCurveInter;

class BezierCurveInterGUI : public ModelGUI
{
public:
	BezierCurveInterGUI(BezierCurveInter& curve);
	virtual void update() override;

private:
	BezierCurveInter& m_curve;
};
