#pragma once

#include "guis/model_guis/model_gui.hpp"

#include <array>

class Point;

class PointGUI : public ModelGUI
{
public:
	PointGUI(Point& point);
	virtual void update() override;

private:
	Point& m_point;

	float m_x{};
	float m_y{};
	float m_z{};

	void getValues();
	void setValues();
};
