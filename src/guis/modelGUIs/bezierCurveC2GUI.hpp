#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "models/curveBase.hpp"

#include <string>
#include <vector>

class BezierCurveC2;

class BezierCurveC2GUI : public ModelGUI
{
public:
	BezierCurveC2GUI(BezierCurveC2& curve);
	virtual void update() override;

private:
	BezierCurveC2& m_curve;

	CurveBase m_base{};
	bool m_renderPolyline{};
	
	std::vector<std::string> m_pointNames{};
	
	void getValues();
	void setValues();
};
