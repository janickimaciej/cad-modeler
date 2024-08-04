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
};
