#pragma once

class Cursor;

class CursorGUI
{
public:
	CursorGUI(Cursor& cursor);
	void update();

private:
	Cursor& m_cursor;

	float m_x{};
	float m_y{};
	float m_z{};

	void getValues();
	void setValues();
};
