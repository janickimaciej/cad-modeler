#pragma once

#include "gui/modelGUIs/modelGUI.hpp"

class BezierCurve;

class BezierCurveGUI : public ModelGUI
{
public:
	BezierCurveGUI(BezierCurve& curve);
	virtual ~BezierCurveGUI() = default;

	virtual void update() override;

private:
	BezierCurve& m_curve;
};
