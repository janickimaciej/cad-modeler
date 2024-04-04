#pragma once

#include "guis/model_guis/model_gui.hpp"

#include <string>
#include <vector>

class BezierCurve;

class BezierCurveGUI : public ModelGUI
{
public:
	BezierCurveGUI(BezierCurve& curve);
	virtual void update() override;

private:
	BezierCurve& m_curve;

	bool m_renderPolyline{};
	std::vector<std::string> m_pointNames{};
	
	void getValues();
	void setValues();
};
