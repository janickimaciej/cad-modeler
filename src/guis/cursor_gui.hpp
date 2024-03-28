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
	float m_prevX{};
	float m_y{};
	float m_prevY{};
	float m_z{};
	float m_prevZ{};
	float m_screenX{};
	float m_prevScreenX{};
	float m_screenY{};
	float m_prevScreenY{};

	void getValues();
	void setValues();
};
