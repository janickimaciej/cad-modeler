#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class Point;

class PointGUI : public ModelGUI
{
public:
	PointGUI(Point& point);
	virtual ~PointGUI() = default;
	virtual void update() override;

private:
	Point& m_point;
};
