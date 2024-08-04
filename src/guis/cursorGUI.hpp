#pragma once

#include <glm/glm.hpp>

#include <string>

class Cursor;

class CursorGUI
{
public:
	CursorGUI(Cursor& cursor);
	void update(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize);

private:
	Cursor& m_cursor;
	static const std::string m_suffix;
	
	void updatePos();
	void updateScreenPos(const glm::mat4& cameraMatrix, const glm::ivec2& windowSize);
};
