#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

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
