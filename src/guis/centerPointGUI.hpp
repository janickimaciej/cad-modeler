#pragma once

class CenterPoint;

class CenterPointGUI
{
public:
	CenterPointGUI(CenterPoint& CenterPoint);
	void update();

private:
	CenterPoint& m_centerPoint;

	float m_x{};
	float m_y{};
	float m_z{};

	void getValues();
	void setValues();
};
