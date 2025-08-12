#pragma once

#include "cameras/camera.hpp"

#include <string>

class Cursor;

class CursorGUI
{
public:
	CursorGUI(Cursor& cursor);
	void update(const Camera& camera);

private:
	Cursor& m_cursor;
	static const std::string m_suffix;

	void updatePos();
	void updateScreenPos(const Camera& camera);
};
