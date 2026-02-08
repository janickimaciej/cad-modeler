#pragma once

class CenterPoint;

class CenterPointGUI
{
public:
	CenterPointGUI(CenterPoint& CenterPoint);
	void update();

private:
	CenterPoint& m_centerPoint;
};
