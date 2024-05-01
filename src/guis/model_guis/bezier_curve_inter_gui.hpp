#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "models/curve_base.hpp"

#include <string>
#include <vector>

class BezierCurveInter;

class BezierCurveInterGUI : public ModelGUI
{
public:
	BezierCurveInterGUI(BezierCurveInter& curve);
	virtual void update() override;

private:
	BezierCurveInter& m_curve;

	CurveBase m_base{};
	bool m_renderPolyline{};
	
	std::vector<std::string> m_pointNames{};
	
	void getValues();
	void setValues();
};
