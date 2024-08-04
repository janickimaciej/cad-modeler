#pragma once

#include <glm/glm.hpp>

#include <string>

class ValueWindow
{
public:
	ValueWindow(const glm::ivec2& windowSize);
	void update();

private:
	const glm::ivec2& m_windowSize;

	virtual std::string name() = 0;
	virtual int width() = 0;
	virtual void input() = 0;
};
